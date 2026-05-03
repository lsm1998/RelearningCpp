#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "greeter.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using greeter::Greeter;
using greeter::HelloReply;
using greeter::HelloRequest;

class GreeterClient {
 public:
  explicit GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(std::move(channel))) {}

  void SayHello(const std::string& user) {
    HelloRequest request;
    request.set_name(user);

    HelloReply reply;
    ClientContext context;

    Status status = stub_->SayHello(&context, request, &reply);
    if (status.ok()) {
      std::cout << "SayHello: " << reply.message() << std::endl;
    } else {
      std::cerr << "SayHello failed: " << status.error_message() << std::endl;
    }
  }

  void LotsOfReplies(const std::string& user) {
    HelloRequest request;
    request.set_name(user);

    ClientContext context;
    auto reader = stub_->LotsOfReplies(&context, request);
    HelloReply reply;
    std::cout << "LotsOfReplies:" << std::endl;
    while (reader->Read(&reply)) {
      std::cout << "  -> " << reply.message() << std::endl;
    }
    Status status = reader->Finish();
    if (!status.ok()) {
      std::cerr << "LotsOfReplies failed: " << status.error_message() << std::endl;
    }
  }

  void LotsOfGreetings(const std::vector<std::string>& users) {
    ClientContext context;
    HelloReply reply;
    auto writer = stub_->LotsOfGreetings(&context, &reply);

    for (const auto& user : users) {
      HelloRequest request;
      request.set_name(user);
      if (!writer->Write(request)) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    writer->WritesDone();
    Status status = writer->Finish();
    if (status.ok()) {
      std::cout << "LotsOfGreetings: " << reply.message() << std::endl;
    } else {
      std::cerr << "LotsOfGreetings failed: " << status.error_message() << std::endl;
    }
  }

  void BidiHello(const std::vector<std::string>& users) {
    ClientContext context;
    auto stream = stub_->BidiHello(&context);

    std::cout << "BidiHello:" << std::endl;

    // Write a few messages
    for (const auto& user : users) {
      HelloRequest request;
      request.set_name(user);
      stream->Write(request);
    }

    // Signal we're done writing
    stream->WritesDone();

    // Read all responses
    HelloReply reply;
    while (stream->Read(&reply)) {
      std::cout << "  -> " << reply.message() << std::endl;
    }

    Status status = stream->Finish();
    if (!status.ok()) {
      std::cerr << "BidiHello failed: " << status.error_message() << std::endl;
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main() {
  GreeterClient client(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  client.SayHello("world");

  client.LotsOfReplies("server stream");

  client.LotsOfGreetings({"Alice", "Bob", "Charlie"});

  client.BidiHello({"foo", "bar", "baz"});

  return 0;
}
