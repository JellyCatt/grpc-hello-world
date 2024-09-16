#include <iostream>
#include <string>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "hello_world.grpc.pb.h"

int main(int argc, char* argv[]) {
  std::string my_name;
  if (argc >= 2) {
    my_name = argv[1];
  } else {
    my_name = "Nghi";
  }
  std::cout << "Greeting with most polite using name: " << my_name << "..." << std::endl;
  std::string server_address("127.0.0.1:55555");
  std::shared_ptr<grpc::Channel> my_channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
  std::unique_ptr<hello_world::Greeting::Stub> my_stub(hello_world::Greeting::NewStub(my_channel));

  hello_world::GreetAsk request;
  request.set_my_name(my_name);

    // Container for the data we expect from the server.
  hello_world::GreetReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

    // The actual RPC.
  grpc::Status status = my_stub->Greet(&context, request, &reply);

    // Act upon its status.
  if (status.ok()) {
    std::cout << reply.polite_reply() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    std::cout <<  "RPC failed: Cannot Greet to the person..." << std::endl;
  }
}