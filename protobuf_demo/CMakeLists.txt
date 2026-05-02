cmake_minimum_required(VERSION 3.16)
project(protobuf_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(protobuf_FORCE_FETCH_DEPENDENCIES ON CACHE BOOL "" FORCE)

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
    PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    PROTOC_EXE protoc
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/protos
    LANGUAGE cpp
)

target_include_directories(protobuf_demo
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf_demo
    PRIVATE
    protobuf::libprotobuf
)