# 构建与运行

本文档用于在新环境从零启动项目。所有命令默认在仓库根目录执行：

```bash
cd RelearningCpp
```

项目使用 CMake。大部分第三方库已放在 `third_party/` 下源码构建；系统只需要安装编译工具、Boost、OpenSSL、libpq、liburing 等基础开发包。

## 目录检查

构建前确认这些目录存在：

```bash
ls third_party/abseil-cpp third_party/protobuf third_party/grpc
ls third_party/hiredis third_party/libpqxx third_party/librdkafka
ls third_party/cpprestsdk third_party/etcd-cpp-apiv3
```

缺少 `third_party/cpprestsdk` 或 `third_party/etcd-cpp-apiv3` 时，补源码：

```bash
git clone --branch v2.10.19 --depth 1 https://github.com/microsoft/cpprestsdk.git third_party/cpprestsdk
git clone --branch v0.15.3 --depth 1 https://github.com/etcd-cpp-apiv3/etcd-cpp-apiv3.git third_party/etcd-cpp-apiv3
```

## Linux

### 依赖

Ubuntu/Debian：

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config git docker.io \
  libboost-all-dev libssl-dev libpq-dev liburing-dev
```

Linux 会构建 `muduo_demo`，依赖 `third_party/muduo` 下的 muduo 头文件和静态库，以及系统 Boost。

### 构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

只构建单个 demo：

```bash
cmake --build build --target etcd_demo -j
cmake --build build --target echo_server -j
cmake --build build --target echo_client -j
```

`io_uring` 目前是单文件示例，单独编译：

```bash
cc io_uring/main.c -o build/io_uring_demo -luring
```

## macOS

### 依赖

Apple Silicon：

```bash
xcode-select --install
brew install cmake pkg-config git boost openssl@3 libpq
```

Intel Mac 将 `/opt/homebrew` 替换为 `/usr/local`。如 CMake 找不到 `libpq`：

```bash
export PKG_CONFIG_PATH="/opt/homebrew/opt/libpq/lib/pkgconfig:$PKG_CONFIG_PATH"
```

### 构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

只构建单个 demo：

```bash
cmake --build build --target etcd_demo -j
cmake --build build --target grpc_server -j
```

macOS 不构建 `muduo_demo` 和 `io_uring`。

## 启动服务

需要 Docker 的 demo：`redis_demo`、`pg_demo`、`kafka_demo`、`etcd_demo`。

```bash
docker run -d --name redis -p 6379:6379 redis:7

docker run -d --name postgres -p 5432:5432 \
  -e POSTGRES_PASSWORD=123456 \
  -e POSTGRES_DB=demo \
  postgres:18

docker run -d --name etcd -p 2379:2379 gcr.io/etcd-development/etcd:v3.6.11 \
  /usr/local/bin/etcd \
  --advertise-client-urls http://0.0.0.0:2379 \
  --listen-client-urls http://0.0.0.0:2379

docker run -d --name kafka -p 9092:9092 apache/kafka:4.2.0
```

重复启动时报容器名已存在时，先清理：

```bash
docker rm -f redis postgres etcd kafka
```

## 运行

基础 demo：

```bash
./build/json_demo/json_demo
./build/log_demo/log_demo
./build/protobuf_demo/protobuf_demo
```

gRPC 需要两个终端：

```bash
./build/grpc_demo/grpc_server
./build/grpc_demo/grpc_client
```

依赖本地服务的 demo：

```bash
./build/redis_demo/redis_demo
./build/pg_demo/pg_demo
./build/kafka_demo/kafka_demo
./build/kafka_demo/kafka_demo consumer
./build/etcd_demo/etcd_demo
```

Linux 专用：

```bash
./build/muduo_demo/echo_server
./build/muduo_demo/echo_client
./build/io_uring_demo
```

## 常用排错

清理重建：

```bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

首次构建 `etcd_demo` 会编译 gRPC/protobuf/cpprestsdk，耗时较长。看到第三方库 warning 通常可以忽略，以最终是否 `Built target ...` 为准。

macOS OpenSSL 找不到时：

```bash
cmake -S . -B build -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3
```

服务连不上时先确认端口：

```bash
docker ps
```
