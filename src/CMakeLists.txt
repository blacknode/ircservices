#
# The ircservices executable: the core that links to the IRC network,
# tracks its state and hosts the loadable modules.
#

set(core_sources
  actions.c     # high-level actions (kill, kick, set topic/modes)
  cache.c       # cache API (cache.h): calls in flight, the redis block
  channels.c    # channel state
  commands.c    # pseudo-client command tables
  conffile.c    # configuration tree, lookups, directive binding
  databases.c   # database table registry
  db.c          # database API (db.h): calls in flight, the database block
  encrypt.c     # password encryption front-end
  events.c      # events: declaring, attaching handlers, emitting
  ignore.c      # flood/ignore accounting
  init.c        # start-up, configuration, shutdown
  language.c    # message catalogs
  log.c         # log file
  main.c        # main loop
  migration.c   # migrations: validating what a module ships
  migration_run.c # migrations: start-up and OperServ MIGRATION
  memory.c      # allocation wrappers (and MEMCHECKS)
  messages.c    # IRC message dispatch tables
  misc.c        # string, mask and time helpers
  modes.c       # user/channel mode tables
  modules.c     # module loader
  p10.c         # P10 protocol (ircu2): numerics, tokens, burst
  process.c     # line splitting and dispatch
  send.c        # outgoing messages
  service.c     # pseudo-clients: introducing, messages, WHOIS
  servers.c     # server tree
  signals.c     # signal handling
  sockets.c     # buffered sockets on top of the event engine
  timeout.c     # timers
  users.c       # user state
  worker.c      # worker threads: pool, dedicated workers, wake-up pipe

  # The PostgreSQL driver behind db.h, and the table store (ported from
  # ircu2's modules/workers/postgres).
  postgres/postgres.c    # the driver db.c registers
  postgres/pg_pool.c     # per-role connection pools and their queue
  postgres/pg_conn.c     # one pooled connection, its deadline, helpers
  postgres/pg_json.c     # PGresult -> JSON rows
  postgres/pg_error.c    # SQLSTATE -> DbError
  postgres/pg_types.c    # DbType -> OID
  postgres/pg_migrate.c  # db_migrate()
  postgres/pg_sync.c     # the main thread's connection: sync work, claim
  postgres/pg_store.c    # the Services tables: schema, load, snapshot
  postgres/pg_save.c     # the Services tables: the save, in a worker

  # The Redis driver behind cache.h (ported from ircu2's
  # modules/workers/redis).
  redis/redis.c          # the driver cache.c registers
  redis/redis_pool.c     # the request queue
  redis/redis_conn.c     # one connection, in its own thread
  redis/redis_client.c   # a synchronous client, one per thread

  # The entity store (store.h): Redis in front of PostgreSQL.
  store/store.c          # working set, reads, writes, prefetches
  store/store_io.c       # the writer thread and the background fetches
)
foreach(engine IN LISTS SERVICES_ENGINES)
  list(APPEND core_sources engine_${engine}.c)
endforeach()

configure_file("${PROJECT_SOURCE_DIR}/cmake/version.c.in"
               "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)

# The configuration file grammar: conf_lexer.l and conf_parser.y build the
# tree that conffile.c serves (see include/conffile.h).  Bison 3.6 is the
# first to let the scanner return the error token (YYerror).
find_package(BISON 3.6 REQUIRED)
find_package(FLEX 2.6 REQUIRED)
bison_target(conf_parser conf_parser.y
  "${CMAKE_CURRENT_BINARY_DIR}/conf_parser.c"
  DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/conf_parser.h"
  COMPILE_FLAGS "-Wall")
flex_target(conf_lexer conf_lexer.l "${CMAKE_CURRENT_BINARY_DIR}/conf_lexer.c")
add_flex_bison_dependency(conf_lexer conf_parser)
# Generated code is not held to the warnings of hand-written code.
set_source_files_properties(
  ${BISON_conf_parser_OUTPUT_SOURCE} ${FLEX_conf_lexer_OUTPUTS}
  PROPERTIES COMPILE_OPTIONS "-Wno-missing-prototypes;-Wno-unused-function;-Wno-unused-but-set-variable")

# The core's own migrations (the migrations and instance tables), compiled
# in like a module's; see cmake/ServicesMigrations.cmake.
services_add_migrations(core "${CMAKE_CURRENT_SOURCE_DIR}/migrations"
  services_core_migrations core_migrations_source)

add_executable(ircservices ${core_sources} ${core_migrations_source}
  ${BISON_conf_parser_OUTPUT_SOURCE} ${FLEX_conf_lexer_OUTPUTS}
  "${CMAKE_CURRENT_BINARY_DIR}/version.c")
set_target_properties(ircservices PROPERTIES
  OUTPUT_NAME "${SERVICES_PROGRAM}"
  ENABLE_EXPORTS ON)                 # modules resolve core symbols from here
target_link_libraries(ircservices PRIVATE services_config m ${CMAKE_DL_LIBS}
  Services::libpq Services::jansson Services::hiredis Threads::Threads)
target_include_directories(ircservices PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"          # conf_internal.h, drivers' headers
  "${CMAKE_CURRENT_BINARY_DIR}")         # conf_parser.h
add_dependencies(ircservices languages)

install(TARGETS ircservices RUNTIME DESTINATION "${SERVICES_BIN_DIR}")
