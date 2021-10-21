cmake_minimum_required(VERSION 3.0)
project(Polynomials C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")

set(SOURCE_FILES
    src/poly.c
    src/poly.h
    src/calc.c
    src/monovector.c
    src/monovector.h
    src/newstring.c
    src/newstring.h
    src/polystack.c
    src/polystack.h)

add_executable(poly ${SOURCE_FILES})

set(TEST_SOURCE_FILES
	src/poly.c
	src/poly.h
	src/poly_test.c)

add_executable(test EXCLUDE_FROM_ALL ${TEST_SOURCE_FILES})
set_target_properties(test PROPERTIES OUTPUT_NAME poly_test)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)
