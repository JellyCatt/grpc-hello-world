#include <unistd.h>

#include <iostream>
#include <string>
#include <memory>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include <grpcpp/alarm.h>
#include "hello_world.grpc.pb.h"

class ClientWriteReactor : public grpc::ClientWriteReactor<hello_world::GreetAsk> {
public:
  ClientWriteReactor(const std::string& name) : greet_count_(0), name_(name) {
    std::string server_address("127.0.0.1:55566");
    channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    stub_ = hello_world::Greeting::NewStub(channel_);

    // std::cout << "Response address: " << std::hex << &response_ << std::endl;
    stub_->async()->StrangeGreet(&context_, &response_, this);

    AddHold();
    GreetHim();
    StartCall();
  }
  void OnWriteDone(bool ok) override {
    std::cout << "His response: " << response_.polite_reply() << std::endl;
    usleep(300000);
    GreetHim();
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
  void GreetHim() {
    if (greet_count_ < 13) {
      greet_count_++;
      std::stringstream request_stream;
      request_stream << name_ << " the " << greet_count_;
      std::cout << "Asking: " << request_stream.str() << std::endl;
      request_.set_my_name(request_stream.str());
      StartWrite(&request_);
    } else {
      StartWritesDone();
      RemoveHold();
    }
  }

  std::string name_;
  hello_world::GreetAsk request_;
  hello_world::GreetReply response_;
  grpc::ClientContext context_;
  std::unique_ptr<hello_world::Greeting::Stub> stub_;
  std::shared_ptr<grpc::Channel> channel_;
  grpc::Alarm alarm_;

  int greet_count_;
};

int main(int argc, char* argv[]) {
  std::string my_name;
  if (argc >= 2) {
    my_name = argv[1];
  } else {
    my_name = "Nghi";
  }
  std::cout << "Greeting with most polite using name: " << my_name << "..." << std::endl;
  ClientWriteReactor my_reader(my_name);
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