# httpd/main carries Mongoose (vendor/mongoose/, never edited), compiled on
# its own -- so that its warnings are its own and its definitions reach
# exactly the two places that need them, itself and server.c -- and linked
# into this module only.  See vendor/mongoose/README.
#
# TLS: MG_TLS_OPENSSL when OpenSSL is there (and SERVICES_HTTP_TLS allows
# it), otherwise Mongoose's own, which is self-contained, TLS 1.3 only and
# takes ECDSA certificates only.
if(NOT TARGET services_mongoose)
  set(mg_tls 3)                                       # MG_TLS_BUILTIN
  set(mg_link "")
  if(SERVICES_HTTP_TLS STREQUAL "none")
    set(mg_tls 0)                                     # MG_TLS_NONE
  elseif(SERVICES_HTTP_TLS STREQUAL "openssl" OR SERVICES_HTTP_TLS STREQUAL "auto")
    if(SERVICES_HTTP_TLS STREQUAL "openssl")
      find_package(OpenSSL REQUIRED)
    else()
      find_package(OpenSSL QUIET)
    endif()
    if(OpenSSL_FOUND)
      set(mg_tls 2)                                   # MG_TLS_OPENSSL
      set(mg_link OpenSSL::SSL OpenSSL::Crypto)
    endif()
  endif()
  message(STATUS "httpd/main: Mongoose with MG_TLS=${mg_tls}"
                 " (0 none, 2 OpenSSL, 3 built-in)")

  add_library(services_mongoose STATIC
    "${PROJECT_SOURCE_DIR}/vendor/mongoose/mongoose.c")
  target_include_directories(services_mongoose PUBLIC
    "${PROJECT_SOURCE_DIR}/vendor/mongoose")
  target_compile_definitions(services_mongoose PUBLIC
    MG_TLS=${mg_tls}
    # The server thread may not touch Services' log, and Mongoose's own
    # writes to stdout.  Failures are reported through the task it posts.
    MG_ENABLE_LOG=0
    # ListenTo takes IPv6 addresses too.
    MG_ENABLE_IPV6=1
    # Nothing here serves a directory; a listing is a thing to opt into.
    MG_ENABLE_DIRLIST=0)
  target_link_libraries(services_mongoose PRIVATE ${mg_link})
  set_target_properties(services_mongoose PROPERTIES
    POSITION_INDEPENDENT_CODE ON)
endif()

set(SERVICES_MODULE_LINK_LIBRARIES services_mongoose)
