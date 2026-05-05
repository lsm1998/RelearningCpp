cmake_minimum_required(VERSION 3.16)
project(RelearningCpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(json_demo)

add_subdirectory(log_demo)

add_subdirectory(protobuf_demo)

add_subdirectory(muduo_demo)

add_subdirectory(pg_demo)

add_subdirectory(kafka_demo)

add_subdirectory(redis_demo)

add_subdirectory(etcd_demo)

add_subdirectory(grpc_demo)