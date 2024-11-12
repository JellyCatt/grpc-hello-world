#include <unistd.h>

#include <iostream>
#include <string>
#include <memory>
#include <chrono>

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
  my_stub->async()->Greet(&context, &request, &reply, [&](grpc::Status stt) {
                                                        if (stt.ok()) {
                                                          std::cout << "Successfully greet" << std::endl;
                                                          std::cout << reply.polite_reply() << std::endl;
                                                        } else {
                                                          std::cout << "Cannot greet that guy..." << std::endl;
                                                        }});

  std::cout << "Right after Greeting" << std::endl;
  for (int i = 0; i < 10; i++) {
    std::cout << "counting: " << i << std::endl;
    sleep(1);
  }
  return 0;
}