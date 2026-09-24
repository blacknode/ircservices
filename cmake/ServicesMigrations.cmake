#
# Building migrations into the thing that owns them.  Ported from ircu2
# (cmake/IrcuMigrations.cmake).
#
# A module with migrations is a module built from a directory -- a project,
# not a single file -- with a migrations/ subdirectory:
#
#   modules/<type>/<name>/migrations/v1_<migration_name>.up.sql
#   modules/<type>/<name>/migrations/v1_<migration_name>.down.sql
#   modules/<type>/<name>/migrations/v2_...
#
# Those .sql files are never copied: not into the build tree, not into the
# install tree.  They are compiled into the shared object as string literals
# (the array `module_migrations'), which is what lets a module carry its
# migrations wherever it is installed, and what makes it impossible for the
# SQL beside a module to be a different version from the code inside it.
#
# The core does the same with its own migrations (src/migrations/), which
# create and maintain the "migrations" table itself.
#
# Nothing here validates the files.  The rules -- the v<N>_<name>.<up|down>.sql
# spelling, an up for every down, versions running 1..N with no gaps -- are
# checked when the module is loaded (src/migration.c), so that whoever loads
# it is told what is wrong with it.  See docs/readme.migrations.

# services_add_migrations(<label> <migrations_dir> <symbol> <generated_var>)
#
# Generates the C source embedding every .sql in <migrations_dir> as the
# array <symbol>, and returns its path in <generated_var> for the caller to
# add to its target's sources.  Regenerates whenever a .sql file changes,
# appears or disappears.
function(services_add_migrations label dir symbol generated_var)
  string(MAKE_C_IDENTIFIER "${label}" safe)
  set(output "${CMAKE_CURRENT_BINARY_DIR}/${safe}_${symbol}.c")

  # CONFIGURE_DEPENDS so that adding a migration is picked up by a build,
  # the same way adding a source is.
  file(GLOB sql_files CONFIGURE_DEPENDS "${dir}/*.sql")

  add_custom_command(
    OUTPUT "${output}"
    COMMAND "${CMAKE_COMMAND}"
            -DSERVICES_MIGRATIONS_DIR=${dir}
            -DSERVICES_SYMBOL=${symbol}
            -DSERVICES_OUTPUT=${output}
            -P "${PROJECT_SOURCE_DIR}/cmake/GenerateMigrations.cmake"
    DEPENDS ${sql_files}
            "${PROJECT_SOURCE_DIR}/cmake/GenerateMigrations.cmake"
    COMMENT "Embedding migrations for ${label}"
    VERBATIM)

  set(${generated_var} "${output}" PARENT_SCOPE)
endfunction()
