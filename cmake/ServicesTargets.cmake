find_program(CLANG_FORMAT clang-format REQUIRED)

file(GLOB_RECURSE FORMAT_SOURCES
    CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/include/*.c
    ${PROJECT_SOURCE_DIR}/include/*.h
)

add_custom_target(format
    COMMAND ${CLANG_FORMAT}
        -i
        ${FORMAT_SOURCES}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Formatting source files"
)
