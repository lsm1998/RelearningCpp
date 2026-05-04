cmake_minimum_required(VERSION 3.10)
project(etcd_demo CXX)

add_executable(etcd_demo
    main.cpp
)

target_include_directories(etcd_demo PRIVATE
    ${PROJECT_SOURCE_DIR}/../third_party
)