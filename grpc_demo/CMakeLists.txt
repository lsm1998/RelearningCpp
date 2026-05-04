cmake_minimum_required(VERSION 3.16)
project(grpc_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------------------------------------------------
# Dependencies: abseil, protobuf (skip if already provided by parent)
# -------------------------------------------------------------------
if(NOT TARGET absl::strings)
    set(BUILD_TESTING OFF)
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/abseil-cpp
        ${CMAKE_CURRENT_BINARY_DIR}/abseil-cpp
        EXCLUDE_FROM_ALL
    )
endif()

if(NOT TARGET protobuf::libprotobuf)
    set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/protobuf
        ${CMAKE_CURRENT_BINARY_DIR}/protobuf
        EXCLUDE_FROM_ALL
    )
endif()

# Minimal protobuf-config.cmake so gRPC's find_package(Protobuf CONFIG) succeeds.
# The actual targets are already created by add_subdirectory above.
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/protobuf-config)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/protobuf-config/protobuf-config.cmake
    "set(Protobuf_FOUND TRUE)\n"
)
set(protobuf_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf-config)

# -------------------------------------------------------------------
# gRPC
# -------------------------------------------------------------------
set(gRPC_PROTOBUF_PROVIDER "package" CACHE STRING "")
set(gRPC_ABSL_PROVIDER "module" CACHE STRING "")
set(gRPC_BUILD_TESTS OFF CACHE BOOL "")
set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF CACHE BOOL "")
set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF CACHE BOOL "")
set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF CACHE BOOL "")
set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF CACHE BOOL "")
set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF CACHE BOOL "")
set(gRPC_BUILD_GRPC_OBJC_PLUGIN OFF CACHE BOOL "")

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc
    ${CMAKE_CURRENT_BINARY_DIR}/grpc
    EXCLUDE_FROM_ALL
)

# 生成proto
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/protos")
set(GENERATE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generate")
set(PROTO_FILE "${PROTO_DIR}/greeter.proto")

file(MAKE_DIRECTORY ${GENERATE_DIR})

set(GENERATED_PB   "${GENERATE_DIR}/greeter.pb.cc" "${GENERATE_DIR}/greeter.pb.h")
set(GENERATED_GRPC "${GENERATE_DIR}/greeter.grpc.pb.cc" "${GENERATE_DIR}/greeter.grpc.pb.h")
set(GENERATED_FILES ${GENERATED_PB} ${GENERATED_GRPC})

add_custom_command(
    OUTPUT ${GENERATED_FILES}
    COMMAND protobuf::protoc
        --cpp_out=${GENERATE_DIR}
        --grpc_out=${GENERATE_DIR}
        --plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
        -I${PROTO_DIR}
        ${PROTO_FILE}
    DEPENDS ${PROTO_FILE} protobuf::protoc grpc_cpp_plugin
    COMMENT "Generating protobuf and gRPC code from greeter.proto"
)

add_executable(grpc_server server.cpp ${GENERATED_FILES})
target_include_directories(grpc_server PRIVATE ${GENERATE_DIR})
target_link_libraries(grpc_server PRIVATE grpc++)

add_executable(grpc_client client.cpp ${GENERATED_FILES})
target_include_directories(grpc_client PRIVATE ${GENERATE_DIR})
target_link_libraries(grpc_client PRIVATE grpc++)
