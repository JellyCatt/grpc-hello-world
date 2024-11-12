#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>


#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "hello_world.grpc.pb.h"
class SyncGreetServerImpl final : public hello_world::Greeting::Service {
  grpc::Status Greet(::grpc::ServerContext* context, const ::hello_world::GreetAsk* request, ::hello_world::GreetReply* response) override {
    response->set_polite_reply(std::string("Politely say hello to ") + request->my_name());
    sleep(1);
    return grpc::Status::OK;
  }
};

int main() {
  std::string server_address("127.0.0.1:55555");
  SyncGreetServerImpl my_sync_server;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&my_sync_server);
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