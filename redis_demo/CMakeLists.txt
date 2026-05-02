cmake_minimum_required(VERSION 3.16)
project(redis_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(DISABLE_TESTS ON CACHE BOOL "" FORCE)
set(ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ENABLE_SSL OFF CACHE BOOL "" FORCE)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/hiredis
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/hiredis
)

add_executable(redis_demo
    main.cpp
)

target_link_libraries(redis_demo PRIVATE
    hiredis
)
