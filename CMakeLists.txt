cmake_minimum_required(VERSION 3.10)
project(JSON_INTEGRATION VERSION 1.0)

message(${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH "/home/nghi/local_grpc")
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

message(${PACKAGE_PREFIX_DIR})
file(COPY ${PACKAGE_PREFIX_DIR}/include
     DESTINATION ${CMAKE_BINARY_DIR}/include)

# Protobuf::libprotobuf
# gRPC::grpc++_reflection
# gRPC::grpc++