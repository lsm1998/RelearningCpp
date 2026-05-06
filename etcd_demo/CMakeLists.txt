cmake_minimum_required(VERSION 3.10)
project(etcd_demo CXX)

if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

if(NOT TARGET absl::strings)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/abseil-cpp
        ${CMAKE_CURRENT_BINARY_DIR}/third_party/abseil-cpp
        EXCLUDE_FROM_ALL
    )
endif()

if(NOT TARGET protobuf::libprotobuf)
    set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/protobuf
        ${CMAKE_CURRENT_BINARY_DIR}/third_party/protobuf
        EXCLUDE_FROM_ALL
    )
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/protobuf-config)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/protobuf-config/protobuf-config.cmake
    "set(Protobuf_FOUND TRUE)\n"
)
set(Protobuf_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf-config CACHE PATH "" FORCE)

if(NOT TARGET grpc++)
    set(gRPC_PROTOBUF_PROVIDER "package" CACHE STRING "" FORCE)
    set(gRPC_ABSL_PROVIDER "module" CACHE STRING "" FORCE)
    set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_OBJC_PLUGIN OFF CACHE BOOL "" FORCE)

    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc
        ${CMAKE_CURRENT_BINARY_DIR}/third_party/grpc
        EXCLUDE_FROM_ALL
    )
endif()

if(TARGET gpr AND NOT TARGET gRPC::gpr)
    add_library(gRPC::gpr ALIAS gpr)
endif()
if(TARGET grpc AND NOT TARGET gRPC::grpc)
    add_library(gRPC::grpc ALIAS grpc)
endif()
if(TARGET grpc++ AND NOT TARGET gRPC::grpc++)
    add_library(gRPC::grpc++ ALIAS grpc++)
endif()
if(TARGET grpc++_reflection AND NOT TARGET gRPC::grpc++_reflection)
    add_library(gRPC::grpc++_reflection ALIAS grpc++_reflection)
endif()
if(TARGET grpc_cpp_plugin AND NOT TARGET gRPC::grpc_cpp_plugin)
    add_executable(gRPC::grpc_cpp_plugin ALIAS grpc_cpp_plugin)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/grpc-config)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/grpc-config/gRPCConfig.cmake
    "set(gRPC_FOUND TRUE)\n"
    "set(gRPC_VERSION 1.0.0)\n"
)
set(gRPC_DIR ${CMAKE_CURRENT_BINARY_DIR}/grpc-config CACHE PATH "" FORCE)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(WERROR OFF CACHE BOOL "" FORCE)
set(CPPREST_EXCLUDE_WEBSOCKETS ON CACHE BOOL "" FORCE)
set(CPPREST_EXCLUDE_COMPRESSION ON CACHE BOOL "" FORCE)
set(CPPREST_HTTP_CLIENT_IMPL none CACHE STRING "" FORCE)
set(CPPREST_HTTP_LISTENER_IMPL none CACHE STRING "" FORCE)
set(CPPREST_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(APPLE AND NOT OPENSSL_ROOT_DIR)
    foreach(OPENSSL_PREFIX
        /opt/homebrew/opt/openssl@3
        /opt/homebrew/opt/openssl
        /usr/local/opt/openssl@3
        /usr/local/opt/openssl
    )
        if(EXISTS ${OPENSSL_PREFIX})
            set(OPENSSL_ROOT_DIR ${OPENSSL_PREFIX} CACHE PATH "" FORCE)
            break()
        endif()
    endforeach()
endif()

if(NOT TARGET cpprest)
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/cpprestsdk
        ${CMAKE_CURRENT_BINARY_DIR}/third_party/cpprestsdk
        EXCLUDE_FROM_ALL
    )
endif()

if(TARGET cpprest)
    find_package(Boost REQUIRED COMPONENTS
        random
        thread
        filesystem
        chrono
        atomic
        date_time
        regex
    )
    find_package(Boost QUIET OPTIONAL_COMPONENTS system)
    target_include_directories(cpprest PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(cpprest PUBLIC
        Boost::random
        Boost::thread
        Boost::filesystem
        Boost::chrono
        Boost::atomic
        Boost::date_time
        Boost::regex
    )
    if(TARGET Boost::system)
        target_link_libraries(cpprest PUBLIC Boost::system)
    endif()
    target_compile_definitions(cpprest PRIVATE
        BOOST_ASIO_ENABLE_OLD_SERVICES
    )
    target_compile_options(cpprest PRIVATE
        $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-std=c++17>
    )
endif()

if(TARGET cpprest AND NOT TARGET cpprestsdk::cpprest)
    add_library(cpprestsdk::cpprest ALIAS cpprest)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpprestsdk-config)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cpprestsdk-config/cpprestsdkConfig.cmake
    "set(cpprestsdk_FOUND TRUE)\n"
)
set(cpprestsdk_DIR ${CMAKE_CURRENT_BINARY_DIR}/cpprestsdk-config CACHE PATH "" FORCE)

set(BUILD_ETCD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_ETCD_CORE_ONLY OFF CACHE BOOL "" FORCE)
set(ETCD_W_STRICT OFF CACHE BOOL "" FORCE)
set(ETCD_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

if(NOT TARGET etcd-cpp-api)
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/etcd-cpp-apiv3
        ${CMAKE_CURRENT_BINARY_DIR}/third_party/etcd-cpp-apiv3
        EXCLUDE_FROM_ALL
    )
endif()

add_executable(etcd_demo
    main.cpp
)

target_include_directories(etcd_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/etcd-cpp-apiv3
)

target_link_libraries(etcd_demo PRIVATE
    etcd-cpp-api
)
