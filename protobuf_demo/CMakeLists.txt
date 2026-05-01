cmake_minimum_required(VERSION 3.16)
project(protobuf_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/protobuf
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/protobuf
)

include_directories(generate)

add_executable(protobuf_demo
    main.cpp
    generate/demo.pb.cc
)

target_link_libraries(protobuf_demo
    PRIVATE
    protobuf::libprotobuf
)