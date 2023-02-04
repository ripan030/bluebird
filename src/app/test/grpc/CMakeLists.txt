find_package(gRPC CONFIG REQUIRED)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

add_library(calc_proto calculator.proto)

target_link_libraries(calc_proto PUBLIC gRPC::grpc++)
target_include_directories(calc_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate(TARGET calc_proto LANGUAGE cpp)
protobuf_generate(TARGET calc_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

add_executable(calc_service calc_service.cpp)
target_link_libraries(calc_service PRIVATE calc_proto)

add_executable(calc_client calc_client.cpp)
target_link_libraries(calc_client PRIVATE calc_proto)

install(TARGETS calc_service calc_client RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
