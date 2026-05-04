cmake_minimum_required(VERSION 3.16)
project(kafka_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RDKAFKA_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(WITH_SSL OFF CACHE BOOL "" FORCE)
set(WITH_SASL OFF CACHE BOOL "" FORCE)
set(WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(WITH_ZSTD OFF CACHE BOOL "" FORCE)
set(WITH_CURL OFF CACHE BOOL "" FORCE)
set(ENABLE_DEVEL OFF CACHE BOOL "" FORCE)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/librdkafka
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/librdkafka
)

add_executable(kafka_demo
    main.cpp
)

target_link_libraries(kafka_demo PRIVATE
    rdkafka++
)
