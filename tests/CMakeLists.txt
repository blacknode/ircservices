#
# Unit tests, run with ctest (ctest --test-dir build).  Each is a program
# built from the sources it tests and nothing else; see each file's
# header for what it checks.
#

# encryption/argon2 against RFC 7693 and RFC 9106.
set(argon2_dir "${PROJECT_SOURCE_DIR}/modules/encryption/argon2")
add_executable(argon2_t argon2_t.c
  "${argon2_dir}/argon2id.c" "${argon2_dir}/pwhash.c")
target_include_directories(argon2_t PRIVATE "${argon2_dir}")
# The checks are assert()s: they must survive a release build.
target_compile_options(argon2_t PRIVATE -UNDEBUG)
add_test(NAME argon2 COMMAND argon2_t)
