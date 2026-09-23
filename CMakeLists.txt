cmake_minimum_required(VERSION 3.16)
project(IRCServices VERSION 5.1.24 LANGUAGES C)
if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckCSourceCompiles)
include(GNUInstallDirs)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Match `configure`'s default of -g -O2 when the user picks nothing.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel)
endif()

# CMake normally defaults to /usr/local. Keep installation local unless the
# caller explicitly selected a prefix with -DCMAKE_INSTALL_PREFIX=...
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH
      "Install prefix (defaults to the current directory)" FORCE)
endif()

set(SERVICES_INSTALL "${CMAKE_INSTALL_PREFIX}/lib")


message(STATUS "Install prefix defaulted to ${CMAKE_INSTALL_PREFIX}")
message(STATUS
  "Install irc services prefix defaulted to ${SERVICES_INSTALL}")


option(SERVICES_SORTED_LISTS "Keep nickname and channel lists sorted" ON)
option(SERVICES_CLEAN_COMPILE
  "Enable conservative warning-clean compilation" ON)
option(SERVICES_MEMCHECKS "Enable memory allocation checks" OFF)
option(SERVICES_SHOWALLOCS "Log allocation activity (requires memchecks)" OFF)
option(SERVICES_DUMPCORE "Write a core file after a crash" OFF)
option(SERVICES_USE_LOCAL_FUNCS "Use compatibility implementations" OFF)

set(SERVICES_PROGRAM ircservices CACHE STRING
  "Name of the IRC Services executable")
set(SERVICES_DATA_DIR
  "${SERVICES_INSTALL}/${SERVICES_PROGRAM}"
  CACHE PATH "IRC Services data installation directory")
set(
  SERVICES_BIN_DIR "${SERVICES_DATA_DIR}/bin"
  CACHE PATH "IRC Services executable installation directory")

check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(sys/select.h HAVE_SYS_SELECT_H)
check_include_file(sys/sysproto.h HAVE_SYS_SYSPROTO_H)
check_type_size("time_t" SIZEOF_TIME_T LANGUAGE C)
check_type_size("gid_t" SIZEOF_GID_T LANGUAGE C)
check_symbol_exists(strerror "string.h" HAVE_STRERROR)
check_symbol_exists(hstrerror "netdb.h" HAVE_HSTRERROR)
check_symbol_exists(snprintf "stdio.h" HAVE_SNPRINTF)
check_symbol_exists(strtok "string.h" HAVE_STRTOK)
check_symbol_exists(strcasecmp "strings.h" HAVE_STRCASECMP)
check_symbol_exists(strdup "string.h" HAVE_STRDUP)
check_symbol_exists(strspn "string.h" HAVE_STRSPN)
check_symbol_exists(strsignal "string.h" HAVE_STRSIGNAL)
check_symbol_exists(gettimeofday "sys/time.h" HAVE_GETTIMEOFDAY)
check_symbol_exists(setgrent "grp.h" HAVE_SETGRENT)
check_symbol_exists(setregid "unistd.h" HAVE_SETREGID)
check_symbol_exists(umask "sys/stat.h" HAVE_UMASK)
check_symbol_exists(fork "unistd.h" HAVE_FORK)
check_symbol_exists(gethostbyname "netdb.h" HAVE_GETHOSTBYNAME)
check_symbol_exists(getrlimit "sys/resource.h" HAVE_GETSETRLIMIT)
check_symbol_exists(crypt "unistd.h;crypt.h" HAVE_CRYPT)
find_library(CRYPT_LIBRARY crypt)
if(CRYPT_LIBRARY)
    set(HAVE_CRYPT 1)
endif()
check_type_size("int" SIZEOF_INT LANGUAGE C)
check_type_size("long" SIZEOF_LONG LANGUAGE C)
check_type_size("void *" SIZEOF_PTR LANGUAGE C)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "OS2")
    set(HAVE_SOCKLEN_T 1)
else()
    set(HAVE_SOCKLEN_T 0)
endif()

foreach(flag HAVE_STDINT_H HAVE_STRINGS_H HAVE_SYS_SELECT_H HAVE_SYS_SYSPROTO_H
        HAVE_STRERROR HAVE_HSTRERROR HAVE_SNPRINTF HAVE_STRTOK HAVE_STRCASECMP
        HAVE_STRDUP HAVE_STRSPN HAVE_STRSIGNAL HAVE_GETTIMEOFDAY HAVE_SETGRENT
        HAVE_SETREGID HAVE_UMASK HAVE_FORK HAVE_GETHOSTBYNAME HAVE_GETSETRLIMIT
        HAVE_CRYPT HAVE_SOCKLEN_T)
    if(${flag})
        set(${flag}_VALUE 1)
    else()
        set(${flag}_VALUE 0)
    endif()
endforeach()
if(SERVICES_SORTED_LISTS)
    set(HASH_SORTED 1)
else()
    set(HASH_SORTED 0)
endif()
foreach(flag SERVICES_CLEAN_COMPILE
  SERVICES_DUMPCORE SERVICES_MEMCHECKS SERVICES_SHOWALLOCS)
    if(${flag})
        set(${flag}_VALUE 1)
    else()
        set(${flag}_VALUE 0)
    endif()
endforeach()
if(SERVICES_USE_LOCAL_FUNCS)
    set(USE_LOCAL_FUNCS_VALUE 1)
else()
    set(USE_LOCAL_FUNCS_VALUE 0)
endif()
if(SIZEOF_TIME_T GREATER_EQUAL 8)
    set(MAX_TIME_T 9223372036854775807)
else()
    set(MAX_TIME_T 2147483647)
endif()
configure_file(cmake/config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
if(SERVICES_CLEAN_COMPILE)
    add_compile_options(-Wall -Wmissing-prototypes)
endif()
if(SERVICES_MEMCHECKS)
    add_compile_definitions(MEMCHECKS)
endif()
if(SERVICES_SHOWALLOCS)
    add_compile_definitions(SHOWALLOCS)
endif()
if(SERVICES_USE_LOCAL_FUNCS)
    add_compile_definitions(USE_LOCAL_FUNCS)
endif()

set(SERVICES_CORE_SOURCES
    src/actions.c src/channels.c src/commands.c src/compat.c src/conffile.c
    src/databases.c src/encrypt.c src/ignore.c src/init.c src/language.c
    src/log.c src/main.c src/memory.c src/messages.c src/misc.c src/modes.c
    src/modules.c src/process.c src/send.c src/servers.c src/signals.c
    src/sockets.c src/timeout.c src/users.c
)
add_executable(${SERVICES_PROGRAM} ${SERVICES_CORE_SOURCES})
target_include_directories(${SERVICES_PROGRAM} PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${SERVICES_PROGRAM} PRIVATE m ${CMAKE_DL_LIBS})
if(CRYPT_LIBRARY)
    target_link_libraries(${SERVICES_PROGRAM} PRIVATE "${CRYPT_LIBRARY}")
endif()
set_target_properties(${SERVICES_PROGRAM} PROPERTIES ENABLE_EXPORTS ON)

function(add_services_module module_dir module_name)
    set(sources
      "${CMAKE_CURRENT_SOURCE_DIR}/modules/${module_dir}/${module_name}.c")
    foreach(extra IN LISTS ARGN)
        list(APPEND sources
          "${CMAKE_CURRENT_SOURCE_DIR}/modules/${module_dir}/${extra}.c")
    endforeach()
    string(REPLACE "-" "_" target_name "${module_dir}_${module_name}")
    add_library(${target_name} MODULE ${sources})
    target_include_directories(${target_name} PRIVATE
      "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include"
      "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_definitions(${target_name}
      PRIVATE MODULE MODULE_ID=${target_name})
    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/modules/${module_dir}/${module_name}.c"
        PROPERTIES COMPILE_DEFINITIONS MODULE_MAIN_FILE)
    set_target_properties(${target_name} PROPERTIES PREFIX ""
      OUTPUT_NAME "${module_name}"
      LIBRARY_OUTPUT_DIRECTORY
      "${CMAKE_CURRENT_BINARY_DIR}/modules/${module_dir}")
    install(TARGETS ${target_name}
      LIBRARY DESTINATION "${SERVICES_DATA_DIR}/modules/${module_dir}")
endfunction()

# Service modules and their additional compilation units, as listed by the
# original modules/*/Makefiles.
add_services_module(chanserv main access check set util)
add_services_module(chanserv access-levels)
add_services_module(chanserv access-xop)
add_services_module(database standard fileutil)
add_services_module(database version4 extsyms)
add_services_module(encryption md5)
add_services_module(encryption unix-crypt)
add_services_module(httpd main util)
add_services_module(httpd auth-ip)
add_services_module(httpd auth-password)
add_services_module(httpd dbaccess)
add_services_module(httpd debug)
add_services_module(httpd redirect)
add_services_module(httpd top-page)
add_services_module(mail main)
add_services_module(mail sendmail)
add_services_module(mail smtp)
add_services_module(memoserv main)
add_services_module(memoserv forward)
add_services_module(memoserv ignore)
add_services_module(misc devnull)
add_services_module(misc helpserv)
add_services_module(misc xml-export)
add_services_module(misc xml-import)
add_services_module(nickserv main collide set util)
add_services_module(nickserv access)
add_services_module(nickserv autojoin)
add_services_module(nickserv link)
add_services_module(nickserv mail-auth)
add_services_module(operserv main maskdata)
add_services_module(operserv akill)
add_services_module(operserv news)
add_services_module(operserv sessions)
add_services_module(operserv sline)
add_services_module(statserv main)

# Protocol backends include common implementation fragments.
foreach(proto bahamut dalnet dreamforge
    hybrid inspircd monkey ptlink ratbox rfc1459
    solidircd trircd ts8 undernet-p9 unreal)
    # Optional protocol features are included directly by the backend source.
    add_services_module(protocol ${proto})
endforeach()

# Version and language files are generated in the build tree.
configure_file(cmake/version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)
target_sources(${SERVICES_PROGRAM} PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/version.c")
configure_file(lang/langstrs.h
  "${CMAKE_CURRENT_BINARY_DIR}/langstrs.h" COPYONLY)

add_executable(langcomp lang/langcomp.c)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lang")
configure_file(lang/index "${CMAKE_CURRENT_BINARY_DIR}/lang/index" COPYONLY)
set(language_files de en_us es fr hu ja_euc ja_sjis nl ru tr)
foreach(language IN LISTS language_files)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/${language}.l"
        "${CMAKE_CURRENT_BINARY_DIR}/lang/${language}.l" COPYONLY)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/lang/${language}"
        COMMAND ${CMAKE_COMMAND} -E make_directory
          "${CMAKE_CURRENT_BINARY_DIR}/lang"
        COMMAND $<TARGET_FILE:langcomp> "${language}.l"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lang"
        DEPENDS langcomp "${CMAKE_CURRENT_BINARY_DIR}/lang/${language}.l"
          "${CMAKE_CURRENT_SOURCE_DIR}/lang/index"
        VERBATIM)
    list(APPEND language_outputs "${CMAKE_CURRENT_BINARY_DIR}/lang/${language}")
endforeach()
add_custom_target(languages ALL DEPENDS ${language_outputs})
install(FILES ${language_outputs} DESTINATION "${SERVICES_DATA_DIR}/languages")

configure_file(data/example-ircservices.conf
  "${CMAKE_CURRENT_BINARY_DIR}/example-${SERVICES_PROGRAM}.conf" COPYONLY)
configure_file(data/example-modules.conf
  "${CMAKE_CURRENT_BINARY_DIR}/example-modules.conf" COPYONLY)
install(TARGETS ${SERVICES_PROGRAM} RUNTIME DESTINATION "${SERVICES_BIN_DIR}")
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/example-${SERVICES_PROGRAM}.conf"
  "${CMAKE_CURRENT_BINARY_DIR}/example-modules.conf" DESTINATION
  "${SERVICES_DATA_DIR}")
install(DIRECTORY data/helpfiles DESTINATION "${SERVICES_DATA_DIR}")
install(FILES docs/ircservices.8 DESTINATION
  "${SERVICES_DATA_DIR}/${CMAKE_INSTALL_MANDIR}/man8")
