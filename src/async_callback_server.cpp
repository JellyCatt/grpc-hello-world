#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>


#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "hello_world.grpc.pb.h"

class CustomUnaryReactor : public grpc::ServerUnaryReactor {
public:
  CustomUnaryReactor(const ::hello_world::GreetAsk* request, ::hello_world::GreetReply* response) {
    response->set_polite_reply(std::string("Hello my man, I'm asyncingly calling your name ") + request->my_name());
    Finish(grpc::Status::OK);
  }
private:
  void OnDone() override {
    std::cout << "Server's OnDone(), deleting this..." << std::endl;
    delete this;
  }
  void OnCancel() override { std::cout << "Server's OnCancel()" << std::endl; }
};

class AsyncGreetServerImpl final : public hello_world::Greeting::CallbackService {
  grpc::ServerUnaryReactor* Greet(
      ::grpc::CallbackServerContext* context, const ::hello_world::GreetAsk* request, ::hello_world::GreetReply* response) override {
        // response->set_polite_reply(std::string("Hello my man, ") + request->my_name());
        // grpc::ServerUnaryReactor *reactor = context->DefaultReactor();
        // reactor->Finish(grpc::Status::OK);
        return new CustomUnaryReactor(request, response);
      }
};
int main() {
  std::string server_address("127.0.0.1:55555");
  AsyncGreetServerImpl my_async_server;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&my_async_server);
  // Finally assemble the server.
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
  // server->Wait();
  for (int i = 0; i < 10; i++) {
    sleep(1);
    std::cout << i << std::endl;
  }
  std::cout << "After wait :)" << std::endl;
  input_thread.join();

  return 0;
}