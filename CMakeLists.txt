cmake_minimum_required(VERSION 3.10)
project(JSON_INTEGRATION VERSION 1.0)

message(${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH "/home/nghi/local_grpc")
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# message(${PACKAGE_PREFIX_DIR})
file(COPY ${PACKAGE_PREFIX_DIR}/include
     DESTINATION ${CMAKE_BINARY_DIR}/include)

# set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
# set(_REFLECTION gRPC::grpc++_reflection)
# set(_GRPC_GRPCPP gRPC::grpc++)

add_subdirectory(src_gen)
add_executable(sync_server src/sync_server.cpp)
target_include_directories(sync_server INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sync_server PUBLIC GeneratedSrc)

add_executable(sync_client src/sync_client.cpp)
target_include_directories(sync_client INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sync_client PUBLIC GeneratedSrc)
# Protobuf::libprotobuf
# gRPC::grpc++_reflection
# gRPC::grpc++