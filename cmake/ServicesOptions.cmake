#
# Cache options.  Every knob of the build is a SERVICES_* cache variable;
# see BUILDING.md for what each one does.
#

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel)
endif()

# Keep installation inside the source tree unless a prefix was chosen.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH
    "Install prefix" FORCE)
endif()

set(SERVICES_PROGRAM ircservices CACHE STRING
  "Name of the IRC Services executable")
set(SERVICES_DATA_DIR "${CMAKE_INSTALL_PREFIX}/lib/${SERVICES_PROGRAM}"
  CACHE PATH "Services directory: configuration, data, modules, languages")
set(SERVICES_BIN_DIR "${SERVICES_DATA_DIR}/bin"
  CACHE PATH "Directory the executable is installed to")

# Sorted lists hash on the first two characters of a nick or channel name
# only, so that walking a table visits it in order.  On a large network a
# common prefix (guest nicks, clones, #chan-*) puts thousands of entries in
# one bucket and every lookup scans it; off, the whole name is hashed.
option(SERVICES_SORTED_LISTS "Keep user and channel lists sorted (slow on large networks)" OFF)
option(SERVICES_WARNINGS     "Build with extra compiler warnings" ON)
option(SERVICES_MEMCHECKS    "Enable memory allocation checks" OFF)
option(SERVICES_SHOWALLOCS   "Log allocation activity (needs MEMCHECKS)" OFF)
option(SERVICES_DUMPCORE     "Write a core file after a crash" OFF)

# Event engine: "auto" builds every engine the platform has and picks the
# best at run time (epoll > kqueue > poll > select), the way ircu does.
set(SERVICES_ENGINE auto CACHE STRING
  "Socket event engine: auto, epoll, kqueue, poll or select")
set_property(CACHE SERVICES_ENGINE PROPERTY STRINGS
  auto epoll kqueue poll select)

if(SERVICES_SHOWALLOCS AND NOT SERVICES_MEMCHECKS)
  message(FATAL_ERROR "SERVICES_SHOWALLOCS requires SERVICES_MEMCHECKS=ON")
endif()
