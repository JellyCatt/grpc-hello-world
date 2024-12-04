#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <exception>
#include <memory>


#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "hello_world.grpc.pb.h"

class CustomServerWriteReactor : public grpc::ServerWriteReactor<hello_world::GreetReply> {
public:
  CustomServerWriteReactor(::grpc::CallbackServerContext* context, const ::hello_world::GreetAsk* request)
      : his_name_(request->my_name()),
        greet_count_(0) {
    std::cout << "CONSTRUCTOR" << std::endl;
    GreetHim();
  }
  void OnWriteDone(bool ok) override {
    if (!ok) {
      std::cout << "OnWriteDone is not ok :(" << std::endl;
    }
    usleep(100000);
    GreetHim();
  }
  void OnDone() override {
    std::cout << "Finished Greeting him hehe" << std::endl;
    delete this;
  }
  void OnCancel() override { std::cout << "RPC Cancelled" << std::endl; }

private:
  void GreetHim() {
    if (greet_count_ < 10) {
      std::stringstream response_stream;
      response_stream << "Hello my man" << his_name_ << " for the " << greet_count_ << " time :)" << std::endl;
      response_.set_polite_reply(response_stream.str());
      greet_count_++;
      StartWrite(&response_);
    } else {
      Finish(grpc::Status::OK);
    }
  }
  std::string his_name_;
  int greet_count_;
  ::hello_world::GreetReply response_;
};

class StreamServer : public hello_world::Greeting::CallbackService {
public:
  grpc::ServerWriteReactor<hello_world::GreetReply>* WeirdGreet(
      ::grpc::CallbackServerContext* context, const ::hello_world::GreetAsk* request) override {
        return new CustomServerWriteReactor(context, request);
      }
};

int main () {
  std::string server_address("127.0.0.1:55555");
  StreamServer my_async_server;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&my_async_server);
  // Finally assemble the server.
  // try {
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  
  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  std::thread input_thread([&](){
    char c = 1;
    while (c != '0') {
      std::cin >> c;
      std::cout << "you entered character: " << (uint16_t)c << std::endl;
    }
    std::cout << "Shutting down server..." << std::endl;
    server->Shutdown();
  });
  input_thread.join();
  // } catch (const std::exception& ex) {
  //   std::cout << "Exception appeared!!!" << std::endl;
  //   std::cout << ex.what();
  // }
  return 0;

}
