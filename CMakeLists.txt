#
# IRC Services -- top-level build.
#
# This file only wires the tree together; each part of it owns its build:
#
#   cmake/ServicesOptions.cmake   cache options (SERVICES_*)
#   cmake/ServicesPlatform.cmake  platform probes, config.h, event engines
#   cmake/ServicesModules.cmake   services_add_module[s]() for modules/
#   src/CMakeLists.txt            the ircservices executable (core)
#   modules/CMakeLists.txt        every loadable module, discovered
#   lang/CMakeLists.txt           langcomp and the compiled language files
#
cmake_minimum_required(VERSION 3.20)
project(IRCServices VERSION 5.1.24 LANGUAGES C)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In-source builds are not supported; use: cmake -S . -B build")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)   # typeof() and friends are used throughout

include(GNUInstallDirs)
include(ServicesOptions)
include(ServicesPlatform)
include(ServicesTargets)

# ---------------------------------------------------------------------------
# services_config: the interface every target of the tree links against.
# It carries the include path, the generated headers and the compile flags,
# so the core, the modules and any future tool see exactly the same thing.
# ---------------------------------------------------------------------------
add_library(services_config INTERFACE)
target_include_directories(services_config INTERFACE
  "${PROJECT_BINARY_DIR}/include"       # config.h
  "${PROJECT_BINARY_DIR}/lang"          # langstrs.h
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}")              # "modules/<type>/<header>.h"
target_compile_definitions(services_config INTERFACE _GNU_SOURCE)
if(SERVICES_WARNINGS)
  target_compile_options(services_config INTERFACE
    -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare
    -Wno-missing-field-initializers -Wmissing-prototypes
    -Werror=implicit-function-declaration -Werror=implicit-int)
endif()

add_subdirectory(lang)
add_subdirectory(src)
add_subdirectory(modules)

# ---------------------------------------------------------------------------
# Data files.
# ---------------------------------------------------------------------------
install(FILES data/example-ircservices.conf
  DESTINATION "${SERVICES_DATA_DIR}"
  RENAME "example-${SERVICES_PROGRAM}.conf")
install(FILES data/example-modules.conf DESTINATION "${SERVICES_DATA_DIR}")
install(FILES docs/ircservices.8
  DESTINATION "${SERVICES_DATA_DIR}/${CMAKE_INSTALL_MANDIR}/man8")

message(STATUS "ircservices ${PROJECT_VERSION}: engine(s) ${SERVICES_ENGINES},"
               " installing to ${SERVICES_DATA_DIR}")
