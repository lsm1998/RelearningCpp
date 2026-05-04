cmake_minimum_required(VERSION 3.10)
project(json_demo CXX)

add_executable(json_demo
    main.cpp
)

target_include_directories(json_demo PRIVATE
    ${PROJECT_SOURCE_DIR}/../third_party
)