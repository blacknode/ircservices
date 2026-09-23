#
# Building loadable modules.
#
# A module is a shared object that Services opens with dlopen() at run
# time.  It links against nothing: the core's symbols are resolved against
# the ircservices executable, which exports its dynamic symbol table
# (ENABLE_EXPORTS on the core target).
#
# The source tree under modules/ is organised by kind of module:
#
#   modules/<type>/<name>.c        a module self-contained in one file
#   modules/<type>/<name>/         a module built from a directory: every
#                                  .c file below it (recursively) is
#                                  compiled in, and .h files are private
#                                  headers.  <name>/<name>.c is the main
#                                  source file of the module.
#   modules/<type>/*.h             headers shared by the modules of <type>
#                                  (and by other modules that talk to
#                                  them); include them from the tree root,
#                                  e.g. #include "modules/nickserv/nickserv.h"
#
# A module is loaded by its <type>/<name>, the string LoadModule takes in
# ircservices.conf ("nickserv/main", "chanserv/access-levels", ...), and
# the build mirrors that: build/modules/<type>/<name>.so, installed under
# <SERVICES_DATA_DIR>/modules/<type>/.
#
# A module that needs something the core knows nothing about -- a system
# library, a definition of its own -- says so in its own CMake fragment:
#
#   modules/<type>/<name>/module.cmake    for a module built from a directory
#   modules/<type>/<name>.cmake           for a single-file module
#
# The fragment is read before the module is built, with
# SERVICES_MODULE_TYPE, SERVICES_MODULE_NAME and SERVICES_MODULE_DIR set,
# and it answers by setting any of:
#
#   SERVICES_MODULE_SKIP                 a reason not to build the module
#   SERVICES_MODULE_LINK_LIBRARIES       libraries to link
#   SERVICES_MODULE_INCLUDE_DIRECTORIES  extra include directories
#   SERVICES_MODULE_COMPILE_DEFINITIONS  extra -D definitions
#   SERVICES_MODULE_COMPILE_OPTIONS      extra compiler flags
#
# modules/CMakeLists.txt calls services_add_modules(), which discovers all
# of this; nothing has to be listed by hand.  services_add_module() below
# builds one module and may be called directly for a module kept elsewhere.
#

# services_add_module(<type> <name>
#                     SOURCES <source>... MAIN <main source>
#                     [DIRECTORY <dir>]
#                     [LINK_LIBRARIES <lib>...] [INCLUDE_DIRECTORIES <dir>...]
#                     [COMPILE_DEFINITIONS <def>...] [COMPILE_OPTIONS <opt>...])
function(services_add_module type name)
  cmake_parse_arguments(arg "" "MAIN;DIRECTORY"
    "SOURCES;LINK_LIBRARIES;INCLUDE_DIRECTORIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS"
    ${ARGN})
  if(arg_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "services_add_module(${type}/${name}): unexpected "
                        "argument(s): ${arg_UNPARSED_ARGUMENTS}")
  endif()
  if(NOT arg_SOURCES OR NOT arg_MAIN)
    message(FATAL_ERROR
      "services_add_module(${type}/${name}): SOURCES and MAIN are required")
  endif()

  # The target name doubles as MODULE_ID, which renames the module's
  # private symbols (see RENAME_SYMBOL in include/modules.h), so it has to
  # be a valid C identifier.
  string(MAKE_C_IDENTIFIER "${type}_${name}" target)

  add_library(${target} MODULE ${arg_SOURCES})
  target_link_libraries(${target} PRIVATE services_config
    ${arg_LINK_LIBRARIES})
  target_compile_definitions(${target} PRIVATE MODULE MODULE_ID=${target}
    ${arg_COMPILE_DEFINITIONS})
  # Exactly one file per module defines the module's bookkeeping symbols
  # (module_version, _this_module_ptr): see include/modules.h.
  set_property(SOURCE "${arg_MAIN}"
    APPEND PROPERTY COMPILE_DEFINITIONS MODULE_MAIN_FILE)
  if(arg_DIRECTORY)
    target_include_directories(${target} PRIVATE "${arg_DIRECTORY}")
  endif()
  if(arg_INCLUDE_DIRECTORIES)
    target_include_directories(${target} PRIVATE ${arg_INCLUDE_DIRECTORIES})
  endif()
  if(arg_COMPILE_OPTIONS)
    target_compile_options(${target} PRIVATE ${arg_COMPILE_OPTIONS})
  endif()

  # "main.so", not "libmain.so": the loader appends ".so" to <type>/<name>.
  set_target_properties(${target} PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    OUTPUT_NAME "${name}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/modules/${type}")
  install(TARGETS ${target}
    LIBRARY DESTINATION "${SERVICES_DATA_DIR}/modules/${type}")
endfunction()

# services_add_modules()
#
# Discovers and builds every module below the calling directory, laid out
# as described at the top of this file.  Hidden entries are skipped at every
# level, and so is anything at the type level that is neither a .c file nor
# a directory.  CONFIGURE_DEPENDS re-runs the globs on every build, so a
# new module is picked up by `cmake --build` without configuring again.
function(services_add_modules)
  set(root "${CMAKE_CURRENT_SOURCE_DIR}")
  file(GLOB types LIST_DIRECTORIES true CONFIGURE_DEPENDS "${root}/*")
  list(SORT types)

  set(built "")
  set(skipped "")
  foreach(typedir IN LISTS types)
    get_filename_component(type "${typedir}" NAME)
    if(type MATCHES "^\\." OR NOT IS_DIRECTORY "${typedir}")
      continue()
    endif()

    file(GLOB entries LIST_DIRECTORIES true CONFIGURE_DEPENDS "${typedir}/*")
    list(SORT entries)
    foreach(entry IN LISTS entries)
      get_filename_component(leaf "${entry}" NAME)
      if(leaf MATCHES "^\\.")
        continue()
      endif()

      if(IS_DIRECTORY "${entry}")
        set(name "${leaf}")
        set(directory "${entry}")
        set(fragment "${entry}/module.cmake")
        set(main "${entry}/${name}.c")
        file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${entry}/*.c")
        list(FILTER sources EXCLUDE REGEX "/\\.")
        list(SORT sources)
        if(NOT EXISTS "${main}")
          message(FATAL_ERROR "${entry}: a module directory needs its main "
                              "source file, ${name}/${name}.c")
        endif()
      elseif(leaf MATCHES "\\.c$")
        string(REGEX REPLACE "\\.c$" "" name "${leaf}")
        set(directory "")
        set(fragment "${typedir}/${name}.cmake")
        set(main "${entry}")
        set(sources "${entry}")
      else()
        continue()
      endif()

      set(SERVICES_MODULE_TYPE "${type}")
      set(SERVICES_MODULE_NAME "${name}")
      set(SERVICES_MODULE_DIR "${directory}")
      foreach(var SKIP LINK_LIBRARIES INCLUDE_DIRECTORIES
                  COMPILE_DEFINITIONS COMPILE_OPTIONS)
        set(SERVICES_MODULE_${var} "")
      endforeach()
      if(EXISTS "${fragment}")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
          "${fragment}")
        include("${fragment}")
      endif()

      if(SERVICES_MODULE_SKIP)
        message(STATUS
          "Module ${type}/${name}: not built (${SERVICES_MODULE_SKIP})")
        list(APPEND skipped "${type}/${name}")
        continue()
      endif()

      services_add_module(${type} ${name}
        SOURCES ${sources} MAIN "${main}" DIRECTORY "${directory}"
        LINK_LIBRARIES ${SERVICES_MODULE_LINK_LIBRARIES}
        INCLUDE_DIRECTORIES ${SERVICES_MODULE_INCLUDE_DIRECTORIES}
        COMPILE_DEFINITIONS ${SERVICES_MODULE_COMPILE_DEFINITIONS}
        COMPILE_OPTIONS ${SERVICES_MODULE_COMPILE_OPTIONS})
      list(APPEND built "${type}/${name}")
    endforeach()
  endforeach()

  list(LENGTH built count)
  message(STATUS "Modules (${count}): ${built}")
  if(skipped)
    message(STATUS "Modules not built: ${skipped}")
  endif()
endfunction()
