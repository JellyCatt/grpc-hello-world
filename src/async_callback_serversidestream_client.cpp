#include <unistd.h>

#include <iostream>
#include <string>
#include <memory>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include "hello_world.grpc.pb.h"

class ReadReactor : public grpc::ClientReadReactor<hello_world::GreetReply> {
public:
  ReadReactor(const std::string& name) {
    std::string server_address("127.0.0.1:55555");
    channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    request_.set_my_name(name);
    stub_ = hello_world::Greeting::NewStub(channel_);
    stub_->async()->WeirdGreet(&context_, &request_, this);
    StartRead(&response_);
    StartCall();
  }
  void OnReadDone(bool ok) override {
    if (ok) {
      std::cout << "He replied back :o :: " << response_.polite_reply() << std::endl;
      StartRead(&response_);
    }
  }
  void OnDone(const grpc::Status& s) override {
    
    if (s.ok()) {
      std::cout << "He is ok" << std::endl;
    } else {
      std::cout << "Status code: " << s.error_code() << std::endl;
      std::cout << "Detail: " << s.error_details() << std::endl;
      std::cout << s.error_message() << std::endl;
    }
    std::cout << "He finished his long greet :)" << std::endl;
  }
private:
  hello_world::GreetAsk request_;
  hello_world::GreetReply response_;
  grpc::ClientContext context_;
  std::unique_ptr<hello_world::Greeting::Stub> stub_;
  std::shared_ptr<grpc::Channel> channel_;
};

int main(int argc, char* argv[]) {
  std::string my_name;
  if (argc >= 2) {
    my_name = argv[1];
  } else {
    my_name = "Nghi";
  }
  std::cout << "Greeting with most polite using name: " << my_name << "..." << std::endl;
  // std::string server_address("127.0.0.1:55555");
  // std::shared_ptr<grpc::Channel> my_channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
  // std::unique_ptr<hello_world::Greeting::Stub> my_stub(hello_world::Greeting::NewStub(my_channel));

  // hello_world::GreetAsk request;
  // request.set_my_name(my_name);

    // Container for the data we expect from the server.
  // hello_world::GreetReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
  // grpc::ClientContext context;

    // The actual RPC.
  // grpc::Status status = my_stub->Greet(&context, request, &reply);
  ReadReactor my_reader(my_name);
  std::cout << "Right after Greeting" << std::endl;
  for (int i = 0; i < 5; i++) {
    std::cout << "Waiting... " << i << std::endl;
    sleep(1);
  }
    // Act upon its status.
  // if (status.ok()) {
  //   std::cout << reply.polite_reply() << std::endl;
  // } else {
  //   std::cout << status.error_code() << ": " << status.error_message()
  //             << std::endl;
  //   std::cout <<  "RPC failed: Cannot Greet to the person..." << std::endl;
  // }
  return 0;
}