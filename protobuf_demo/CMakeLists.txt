cmake_minimum_required(VERSION 3.16)
project(protobuf_demo LANGUAGES CXX)

# 本地 Abseil，避免每次从 GitHub 下载
set(BUILD_TESTING OFF)
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/abseil-cpp
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/abseil-cpp
    EXCLUDE_FROM_ALL
)

set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/protobuf
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/protobuf
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/protobuf/cmake/protobuf-generate.cmake)

add_executable(protobuf_demo
    main.cpp
)

file(GLOB_RECURSE PROTO_FILES protos/*.proto)

protobuf_generate(
    TARGET protobuf_demo
    PROTOS ${PROTO_FILES}
    PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generate
    PROTOC_EXE protoc
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/protos
    LANGUAGE cpp
)

target_include_directories(protobuf_demo
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/generate
)

target_link_libraries(protobuf_demo
    PRIVATE
    protobuf::libprotobuf
)