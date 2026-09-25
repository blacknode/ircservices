#
# Platform probes and the generated config.h.
#
# The code base assumes a C99 compiler and a POSIX system, so only what
# genuinely varies between platforms is probed here: type sizes,
# getentropy(3) and which socket event engines exist.
#

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)

check_type_size("long"   SIZEOF_LONG   LANGUAGE C)
check_type_size("time_t" SIZEOF_TIME_T LANGUAGE C)
check_type_size("gid_t"  SIZEOF_GID_T  LANGUAGE C)

# getentropy(3), for the salts of encryption/argon2 (which reads
# /dev/urandom where it is missing).
set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
check_symbol_exists(getentropy "unistd.h" HAVE_GETENTROPY)
if(NOT HAVE_GETENTROPY)
  unset(HAVE_GETENTROPY CACHE)
  check_symbol_exists(getentropy "sys/random.h" HAVE_GETENTROPY)
endif()
unset(CMAKE_REQUIRED_DEFINITIONS)

# ---------------------------------------------------------------------------
# Event engines (src/engine_*.c).  Each one that the platform supports is
# compiled in; engine_select.c is the fallback that always is.
# ---------------------------------------------------------------------------
check_symbol_exists(epoll_create1 "sys/epoll.h" HAVE_EPOLL)
check_symbol_exists(kqueue "sys/types.h;sys/event.h;sys/time.h" HAVE_KQUEUE)
check_symbol_exists(poll "poll.h" HAVE_POLL)

set(SERVICES_ENGINES "")
foreach(engine epoll kqueue poll)
  string(TOUPPER "${engine}" ENGINE)
  if(HAVE_${ENGINE} AND SERVICES_ENGINE MATCHES "^(auto|${engine})$")
    list(APPEND SERVICES_ENGINES ${engine})
  endif()
endforeach()
list(APPEND SERVICES_ENGINES select)
if(NOT SERVICES_ENGINE STREQUAL "auto")
  list(FIND SERVICES_ENGINES "${SERVICES_ENGINE}" found)
  if(found EQUAL -1)
    message(FATAL_ERROR "SERVICES_ENGINE=${SERVICES_ENGINE} is not "
                        "available on this platform")
  endif()
endif()

foreach(engine epoll kqueue poll select)
  string(TOUPPER "${engine}" ENGINE)
  if(engine IN_LIST SERVICES_ENGINES)
    set(ENGINE_${ENGINE} 1)
  else()
    set(ENGINE_${ENGINE} 0)
  endif()
endforeach()

# ---------------------------------------------------------------------------
# config.h
# ---------------------------------------------------------------------------
if(SIZEOF_TIME_T GREATER_EQUAL 8)
  set(MAX_TIME_T "INT64_MAX")
else()
  set(MAX_TIME_T "INT32_MAX")
endif()

foreach(flag SERVICES_SORTED_LISTS SERVICES_WARNINGS SERVICES_DUMPCORE
             SERVICES_MEMCHECKS SERVICES_SHOWALLOCS HAVE_GETENTROPY)
  if(${flag})
    set(${flag}_VALUE 1)
  else()
    set(${flag}_VALUE 0)
  endif()
endforeach()

configure_file("${PROJECT_SOURCE_DIR}/cmake/config.h.in"
               "${PROJECT_BINARY_DIR}/include/config.h" @ONLY)
