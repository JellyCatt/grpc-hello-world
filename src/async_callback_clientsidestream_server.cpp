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

class CustomServerReadReactor : public grpc::ServerReadReactor<hello_world::GreetAsk> {
public:
  CustomServerReadReactor(hello_world::GreetReply* response) : response_(response), greet_count_(0) {
    StartRead(&request_);
  }
  void OnReadDone(bool ok) override {
    
      greet_count_++;
      std::stringstream response_stream;
      response_stream << "Hello " << request_.my_name() << " for the " << greet_count_ << " time :)";

      std::cout << "Got request: " << request_.my_name() << std::endl;
      std::cout << "Going to response: " << response_stream.str() << std::endl;

      response_->set_polite_reply(response_stream.str());
    // if (ok) {
      // StartRead(&request_);
    // } else {
      std::cout << "I'm done :)" << std::endl;
      Finish(grpc::Status::OK);
    // }
  }
  void OnDone() override {
    std::cout << "Finished Greeting him hehe" << std::endl;
    delete this;
  }

  void OnCancel() override { std::cout << "RPC Cancelled" << std::endl; }
private:
  hello_world::GreetReply* response_;
  hello_world::GreetAsk request_;
  int greet_count_;
};

// class CustomServerAsyncRead : public grpc::ServerAsyncReader<hello_world::GreetReply {};

class StreamReadServer : public hello_world::Greeting::CallbackService {
public:
  ::grpc::ServerReadReactor< ::hello_world::GreetAsk>* StrangeGreet(
      ::grpc::CallbackServerContext* context, ::hello_world::GreetReply* response) {
    return new CustomServerReadReactor(response);
  }
};

int main () {
  std::string server_address("127.0.0.1:55566");
  StreamReadServer my_async_server;

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