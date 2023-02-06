find_package(gRPC CONFIG REQUIRED)
find_package(sdbus-c++ REQUIRED)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

add_library(chat_proto chat.proto)

target_link_libraries(chat_proto PUBLIC gRPC::grpc++)
target_include_directories(chat_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate(TARGET chat_proto LANGUAGE cpp)
protobuf_generate(TARGET chat_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

add_executable(chat_client chat_client.cpp)
target_link_libraries(chat_client PRIVATE chat_proto SDBusCpp::sdbus-c++)

install(TARGETS chat_client RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
