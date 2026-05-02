cmake_minimum_required(VERSION 3.16)
project(pg_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_TEST OFF CACHE BOOL "" FORCE)
set(SKIP_BUILD_TEST ON CACHE BOOL "" FORCE)

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
