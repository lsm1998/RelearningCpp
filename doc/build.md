# 构建与运行

项目使用 CMake，根目录会构建各个 `*_demo`。Linux 会额外构建 `muduo_demo` 和 `io_uring`。

## Linux

### 依赖

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libpq-dev liburing-dev libboost-all-dev libssl-dev
```

`muduo_demo` 在 Linux 下构建，依赖 Boost。`etcd_demo` 使用 `third_party/etcd-cpp-apiv3` 和 `third_party/cpprestsdk` 源码，仍需要系统 Boost/OpenSSL 开发包。

### 构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

### 运行

```bash
./build/json_demo/json_demo
./build/log_demo/log_demo
./build/protobuf_demo/protobuf_demo

./build/grpc_demo/grpc_server
./build/grpc_demo/grpc_client

./build/redis_demo/redis_demo
./build/pg_demo/pg_demo
./build/kafka_demo/kafka_demo
./build/kafka_demo/kafka_demo consumer
./build/etcd_demo/etcd_demo

./build/muduo_demo/echo_server
./build/muduo_demo/echo_client
```

`io_uring` 单独编译运行：

```bash
cc io_uring/main.c -o build/io_uring_demo -luring
./build/io_uring_demo
```

## macOS

### 依赖

```bash
xcode-select --install
brew install cmake pkg-config libpq boost openssl@3
```

如 CMake 找不到 `libpq`：

```bash
export PKG_CONFIG_PATH="/opt/homebrew/opt/libpq/lib/pkgconfig:$PKG_CONFIG_PATH"
```

Intel Mac 可把 `/opt/homebrew` 换成 `/usr/local`。

### 构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

### 运行

```bash
./build/json_demo/json_demo
./build/log_demo/log_demo
./build/protobuf_demo/protobuf_demo

./build/grpc_demo/grpc_server
./build/grpc_demo/grpc_client

./build/redis_demo/redis_demo
./build/pg_demo/pg_demo
./build/kafka_demo/kafka_demo
./build/kafka_demo/kafka_demo consumer
./build/etcd_demo/etcd_demo
```

macOS 不构建 `muduo_demo` 和 `io_uring`。

## 服务依赖

部分 demo 运行前需要本地服务：

```bash
docker run -d --name redis -p 6379:6379 redis:latest
docker run -d --name kafka -p 9092:9092 apache/kafka:latest
docker run -d --name etcd -p 2379:2379 quay.io/coreos/etcd:latest \
  /usr/local/bin/etcd --advertise-client-urls http://0.0.0.0:2379 \
  --listen-client-urls http://0.0.0.0:2379
docker run -d --name postgres -p 5432:5432 \
  -e POSTGRES_PASSWORD=123456 -e POSTGRES_DB=demo postgres:latest
```
