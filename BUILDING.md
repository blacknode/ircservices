# Building and running IRC Services

IRC Services links to an **ircu2** network (Undernet ircu 2.10+, P10
protocol) as a services server. P10 is built into the core; there is no
protocol module to choose or load.

## Requirements

* CMake 3.20+ (3.21+ for the presets), Ninja or Make
* A C99 compiler (GCC or Clang) on a POSIX system
* GNU Bison 3.6+ and flex 2.6+ (the configuration file parser)
* Optional: `crypt(3)` for the `encryption/unix-crypt` module

## Build

```sh
cmake -S . -B build              # or: cmake --preset default
cmake --build build -j
cmake --install build            # installs into SERVICES_DATA_DIR
```

In-source builds are refused. Presets (`CMakePresets.json`):

| preset    | what it is                                               |
|-----------|----------------------------------------------------------|
| `default` | RelWithDebInfo in `build/` (what `.clangd` points at)    |
| `dev`     | Debug + `SERVICES_MEMCHECKS`                              |
| `asan`    | Debug + AddressSanitizer/UBSan; run with `ASAN_OPTIONS=detect_odr_violation=0` (every module defines `module_config`) |
| `release` | Release                                                  |

### Options

| cache variable          | default                           | meaning |
|-------------------------|-----------------------------------|---------|
| `SERVICES_PROGRAM`      | `ircservices`                     | executable name |
| `SERVICES_DATA_DIR`     | `<prefix>/lib/<program>`          | configuration, databases, modules, languages |
| `SERVICES_BIN_DIR`      | `<data dir>/bin`                  | where the executable goes |
| `SERVICES_ENGINE`       | `auto`                            | socket event engine: `auto`, `epoll`, `kqueue`, `poll`, `select` |
| `SERVICES_SORTED_LISTS` | `ON`                              | keep nick/channel lists sorted |
| `SERVICES_WARNINGS`     | `ON`                              | `-Wall -Wextra` and friends |
| `SERVICES_MEMCHECKS`    | `OFF`                             | allocation checks |
| `SERVICES_SHOWALLOCS`   | `OFF`                             | log allocations (needs MEMCHECKS) |
| `SERVICES_DUMPCORE`     | `OFF`                             | leave a core file after a crash |

`CMAKE_INSTALL_PREFIX` defaults to the source directory.

## Source layout

```
CMakeLists.txt          wires the tree together
cmake/                  ServicesOptions / ServicesPlatform / ServicesModules,
                        config.h.in, version.c.in
include/                core headers; every header includes what it uses
                        (types.h is the bottom of the include graph)
src/                    the core executable (src/CMakeLists.txt)
  p10.c                 P10 protocol: numerics, tokens, burst, accounts
  sockets.c             buffered sockets, callbacks
  engine.h, engine_*.c  event engines (epoll, kqueue, poll, select)
  conf_lexer.l,         configuration file scanner and grammar (flex/bison);
  conf_parser.y,        conffile.c holds the parsed tree, its lookup API
  conffile.c            and the binding of directive tables
modules/                loadable modules (modules/CMakeLists.txt)
lang/                   message catalogs; langstrs.h is generated from lang/index
data/                   example configuration files
docs/                   documentation, one readme.<topic> file per topic
```

### Event engines

As in ircu, waiting for socket activity is delegated to an *engine*, a
table of operations (`src/engine.h`) implemented by one backend per file.
Every engine the platform has is compiled in, and the first that
initializes is used, in the order epoll, kqueue, poll, select. The log
says which one at start-up (`sockets: using the epoll event engine`).

### Modules

Modules are discovered, not listed (`cmake/ServicesModules.cmake`, modelled
on ircu's `IrcuModules.cmake`):

```
modules/<type>/<name>.c     a single-file module
modules/<type>/<name>/      a module built from every .c below it;
                            <name>/<name>.c is its main file
modules/<type>/*.h          headers shared by the modules of <type>;
                            include them as "modules/<type>/<file>.h"
```

Either way the result is `build/modules/<type>/<name>.so`, loaded with
`loadmodule <type>/<name>;`. To add a module, create the file or directory
and rebuild: the globs pick it up. A module that needs a library or a
flag declares it in `<name>.cmake` (or `<name>/module.cmake`); see
`modules/encryption/unix-crypt.cmake`.

## Linking to ircu2

In `ircservices.conf`:

```
uplink {
    host = 127.0.0.1;
    port = 4400;
    password = "linkpass";
};
serverinfo {
    name = "services.example.net";
    numeric = 4094;             # unique on the network, 0..4095
    ...
};
```

In the uplink's `ircd.conf`:

```
Connect {
        name = "services.example.net";
        host = "127.0.0.1";
        password = "linkpass";
        class = "Server";
};
UWorld {
        name = "services.example.net";
};
```

The `UWorld` block is required: it is what lets Services send `ACCOUNT`
and change modes or kick users without being channel operators. Services
link with the `+s` server flag, so ircu2 does not require them to run its
module set; they do answer the network's `MODULE PREPARE`.

What P10 changes for the service modules:

* **Accounts.** When NickServ identifies a user, Services log the user in
  with `ACCOUNT <user> <nick> <stamp>` (so the nick must fit `ACCOUNTLEN`,
  12). The account id is the user's Services stamp, so after a Services
  restart a user who was identified is recognised from the NICK burst.
  ircu cannot remove an account, so un-identifying does not take it back.
* **Channel modes and kicks** are sent by the services server, not by
  ChanServ, because ircu bounces a MODE or KICK from a non-op user. The
  pseudo-clients are `+kS` (channel service, network service).
* **AKILL / SZLINE** become G-lines (`GL * +user@host`). Set
  `ImmediatelySendAutokill` in the `operserv/akill` module block to push them to the network
  at once instead of when a matching user connects.
* **JUPE** uses ircu's `JUPE`, which lasts at most 7 days.
* No `SVSNICK`/`SVSJOIN`: `NSForceNickChange` and `nickserv/autojoin`
  are not available; NickServ enforces with a KILL and an enforcer.

## Running

```sh
cd <SERVICES_DATA_DIR>
cp example-ircservices.conf ircservices.conf   # edit
cp example-modules.conf modules.conf           # edit (ServicesRoot, ...)
bin/ircservices -nofork -debug                 # foreground, verbose log
```

`ircservices.conf` is parsed (with everything it `include`s, such as
`modules.conf`) before any module is loaded; errors are reported as
`file:line: message` and stop the start-up. The syntax and every core
setting are described in `docs/readme.config`, the module settings in
`docs/readme.modules`.

`misc/helpserv` needs a directory of help files (`HelpDir`); none are
shipped.
