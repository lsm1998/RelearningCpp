cmake_minimum_required(VERSION 3.10)
project(json_demo CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(json_demo
    main.cpp
)

target_include_directories(json_demo PRIVATE
    ${PROJECT_SOURCE_DIR}/../third_party
)