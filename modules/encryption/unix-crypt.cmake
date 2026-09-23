# encryption/unix-crypt wraps the system crypt(3).
if(NOT HAVE_CRYPT)
  set(SERVICES_MODULE_SKIP "crypt(3) not available")
elseif(CRYPT_LIBRARY)
  set(SERVICES_MODULE_LINK_LIBRARIES "${CRYPT_LIBRARY}")
endif()
