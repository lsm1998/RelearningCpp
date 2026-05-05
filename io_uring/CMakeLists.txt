cmake_minimum_required(VERSION 3.10)
project(io_uring C)

add_executable(io_uring main.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBURING REQUIRED liburing)
target_include_directories(io_uring PRIVATE ${LIBURING_INCLUDE_DIRS})
target_link_libraries(io_uring PRIVATE ${LIBURING_LINK_LIBRARIES})
