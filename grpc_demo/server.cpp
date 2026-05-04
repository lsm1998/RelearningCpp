#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "greeter.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using greeter::Greeter;
using greeter::HelloReply;
using greeter::HelloRequest;

class GreeterServiceImpl final : public Greeter::Service {
public:
    Status SayHello(ServerContext * /*context*/, const HelloRequest *request,
                    HelloReply *reply) override {
        reply->set_message("Hello " + request->name());
        return Status::OK;
    }

    Status LotsOfReplies(ServerContext * /*context*/, const HelloRequest *request,
                         ServerWriter<HelloReply> *writer) override {
        for (int i = 1; i <= 5; i++) {
            HelloReply reply;
            reply.set_message("Hello " + request->name() + " #" + std::to_string(i));
            writer->Write(reply);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        return Status::OK;
    }

    Status LotsOfGreetings(ServerContext * /*context*/,
                           ServerReader<HelloRequest> *reader,
                           HelloReply *reply) override {
        HelloRequest request;
        std::string names;
        while (reader->Read(&request)) {
            if (!names.empty()) names += ", ";
            names += request.name();
        }
        reply->set_message("Hello " + names);
        return Status::OK;
    }

    Status BidiHello(ServerContext * /*context*/,
                     ServerReaderWriter<HelloReply, HelloRequest> *stream) override {
        HelloRequest request;
        while (stream->Read(&request)) {
            HelloReply reply;
            reply.set_message("Echo: " + request.name());
            stream->Write(reply);
        }
        return Status::OK;
    }
};

void RunServer() {
    const std::string address("0.0.0.0:50051");
    GreeterServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr server(builder.BuildAndStart());
    std::cout << "Server listening on " << address << std::endl;

    server->Wait();
}

int main() {
    RunServer();
    return 0;
}
