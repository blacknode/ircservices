# Building with CMake

The CMake build keeps all generated files in the build directory. From the
source directory, configure and build with:

```sh
cmake -S . -B build
cmake --build build
```

Install with `cmake --install build`. By default, files are installed under
the current directory (`sbin` for the program and `share/ircservices` for
data). Choose another destination with `cmake --install build --prefix /path`
or set `-DCMAKE_INSTALL_PREFIX=/path` when configuring.

Useful CMake options:

* `-DSERVICES_PROGRAM=name` changes the executable name.
* `-DSERVICES_BIN_DIR=path` and `-DSERVICES_DATA_DIR=path` set the program
  and data installation paths directly.
* `-DSERVICES_SORTED_LISTS=OFF` disables sorted nickname and channel lists.
* `-DSERVICES_CLEAN_COMPILE=OFF` disables the extra compiler warnings.
* `-DSERVICES_MEMCHECKS=ON` enables allocation checks; pair it with
  `-DSERVICES_SHOWALLOCS=ON` to log allocation activity.
* `-DSERVICES_DUMPCORE=ON` enables core dumps after crashes.
* `-DSERVICES_BUILD_TOOLS=OFF` skips `convert-db` and `ircservices-chk`.

The build creates dynamically loadable modules by default. Install the
provided example configuration files and edit them before starting Services.
