cmake_minimum_required(VERSION 3.10)

add_library(GeneratedSrc hello_world.grpc.pb.cc hello_world.pb.cc)
target_include_directories(GeneratedSrc PUBLIC ${PROJECT_BINARY_DIR})
target_include_directories(GeneratedSrc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GeneratedSrc PUBLIC 
  # ${_PROTOBUF_LIBPROTOBUF}
  protobuf::libprotobuf
  # ${_REFLECTION}
  gRPC::grpc++_reflection
  # ${_GRPC_GRPCPP}
  gRPC::grpc++)