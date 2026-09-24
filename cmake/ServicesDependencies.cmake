#
# Libraries the core links against: PostgreSQL (libpq) for the database,
# jansson for the rows db.h hands back as JSON, hiredis for the cache, and
# the system's threads for the workers that talk to both.
#
# All four are required: the database is where Services keep their data,
# and the drivers are part of the core (src/postgres/, src/redis/).
#
#   Debian, Ubuntu:   apt install libpq-dev libjansson-dev libhiredis-dev
#   Red Hat, Fedora:  dnf install libpq-devel jansson-devel hiredis-devel
#   Arch, Manjaro:    pacman -S postgresql-libs jansson hiredis
#   FreeBSD:          pkg install postgresql16-client jansson hiredis
#   macOS (Homebrew): brew install libpq jansson hiredis
#
# Each is looked up with pkg-config first and by header and library name
# otherwise (a Red Hat system with only libpq-devel has no libpq.pc, for
# instance), and ends up as an imported target:
#
#   Services::libpq  Services::jansson  Services::hiredis  Threads::Threads
#

find_package(PkgConfig QUIET)
find_package(Threads REQUIRED)

# services_find_library(<target> <pkg-config name> <header> <library>
#                       [PATH_SUFFIXES <suffix>...])
function(services_find_library target pcname header library)
  cmake_parse_arguments(arg "" "" "PATH_SUFFIXES" ${ARGN})
  if(TARGET Services::${target})
    return()
  endif()

  if(PkgConfig_FOUND)
    pkg_check_modules(SERVICES_${target} QUIET IMPORTED_TARGET ${pcname})
  endif()
  if(SERVICES_${target}_FOUND)
    add_library(Services::${target} ALIAS PkgConfig::SERVICES_${target})
    message(STATUS "Found ${pcname} ${SERVICES_${target}_VERSION}")
    return()
  endif()

  find_path(SERVICES_${target}_INCLUDE_DIR ${header}
    PATH_SUFFIXES ${arg_PATH_SUFFIXES})
  find_library(SERVICES_${target}_LIBRARY NAMES ${library})
  if(NOT SERVICES_${target}_INCLUDE_DIR OR NOT SERVICES_${target}_LIBRARY)
    message(FATAL_ERROR
      "${pcname} was not found (${header}, lib${library}); it is required.\n"
      "Install its development package: see cmake/ServicesDependencies.cmake.")
  endif()
  add_library(Services::${target} UNKNOWN IMPORTED)
  set_target_properties(Services::${target} PROPERTIES
    IMPORTED_LOCATION "${SERVICES_${target}_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SERVICES_${target}_INCLUDE_DIR}")
  message(STATUS "Found ${pcname}: ${SERVICES_${target}_LIBRARY}")
endfunction()

services_find_library(libpq libpq libpq-fe.h pq
  PATH_SUFFIXES postgresql pgsql)
services_find_library(jansson jansson jansson.h jansson)
services_find_library(hiredis hiredis hiredis/hiredis.h hiredis)
