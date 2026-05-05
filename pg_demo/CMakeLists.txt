cmake_minimum_required(VERSION 3.16)
project(pg_demo LANGUAGES CXX)

set(BUILD_TEST OFF CACHE BOOL "" FORCE)
set(SKIP_BUILD_TEST ON CACHE BOOL "" FORCE)

if(APPLE AND EXISTS "/opt/homebrew/opt/libpq/lib/pkgconfig")
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/opt/libpq/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

find_package(PostgreSQL QUIET)
if(NOT TARGET PostgreSQL::PostgreSQL)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBPQ REQUIRED IMPORTED_TARGET libpq)

    add_library(PostgreSQL::PostgreSQL INTERFACE IMPORTED)
    target_link_libraries(PostgreSQL::PostgreSQL INTERFACE PkgConfig::LIBPQ)
    set(PostgreSQL_FOUND TRUE)
    set(PostgreSQL_INCLUDE_DIRS ${LIBPQ_INCLUDE_DIRS})
endif()

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libpqxx
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/libpqxx
)

add_executable(pg_demo
    main.cpp
)

target_link_libraries(pg_demo PRIVATE
    pqxx
)
