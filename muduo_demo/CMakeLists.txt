cmake_minimum_required(VERSION 3.10)
project(muduo_echo_server CXX)

# muduo 安装路径
set(MUDUO_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/muduo")

include_directories(${MUDUO_INSTALL_PREFIX}/include)
link_directories(${MUDUO_INSTALL_PREFIX}/lib)

add_executable(echo_server server.cpp)

add_executable(echo_client client.cpp)

target_link_libraries(echo_server
    muduo_net
    muduo_base
    pthread
)

target_link_libraries(echo_client
    pthread
)

# 运行时优先到 muduo 安装目录找动态库
set_target_properties(echo_server PROPERTIES
    BUILD_RPATH "${MUDUO_INSTALL_PREFIX}/lib"
)