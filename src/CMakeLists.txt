#
# The ircservices executable: the core that links to the IRC network,
# tracks its state and hosts the loadable modules.
#

set(core_sources
  actions.c     # high-level actions (kill, kick, set topic/modes)
  channels.c    # channel state
  commands.c    # pseudo-client command tables
  conffile.c    # configuration file parser
  databases.c   # database table registry
  encrypt.c     # password encryption front-end
  ignore.c      # flood/ignore accounting
  init.c        # start-up, configuration, shutdown
  language.c    # message catalogs
  log.c         # log file
  main.c        # main loop
  memory.c      # allocation wrappers (and MEMCHECKS)
  messages.c    # IRC message dispatch tables
  misc.c        # string, mask and time helpers
  modes.c       # user/channel mode tables
  modules.c     # module loader and callbacks
  p10.c         # P10 protocol (ircu2): numerics, tokens, burst
  process.c     # line splitting and dispatch
  send.c        # outgoing messages
  servers.c     # server tree
  signals.c     # signal handling
  sockets.c     # buffered sockets on top of the event engine
  timeout.c     # timers
  users.c       # user state
)
foreach(engine IN LISTS SERVICES_ENGINES)
  list(APPEND core_sources engine_${engine}.c)
endforeach()

configure_file("${PROJECT_SOURCE_DIR}/cmake/version.c.in"
               "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)

add_executable(ircservices ${core_sources}
  "${CMAKE_CURRENT_BINARY_DIR}/version.c")
set_target_properties(ircservices PROPERTIES
  OUTPUT_NAME "${SERVICES_PROGRAM}"
  ENABLE_EXPORTS ON)                 # modules resolve core symbols from here
target_link_libraries(ircservices PRIVATE services_config m ${CMAKE_DL_LIBS})
target_include_directories(ircservices PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
add_dependencies(ircservices languages)

install(TARGETS ircservices RUNTIME DESTINATION "${SERVICES_BIN_DIR}")
