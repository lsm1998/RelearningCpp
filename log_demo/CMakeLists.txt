cmake_minimum_required(VERSION 3.16)
project(log_demo LANGUAGES CXX)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE_HO OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS_HO OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/spdlog
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/spdlog
)

add_executable(log_demo
    main.cpp
)

target_link_libraries(log_demo
    PRIVATE
    spdlog::spdlog
)