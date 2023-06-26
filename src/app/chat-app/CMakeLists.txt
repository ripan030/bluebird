find_package(gRPC CONFIG REQUIRED)
find_package(sdbus-c++ REQUIRED)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

add_library(chat_proto chat.proto)

target_link_libraries(chat_proto PUBLIC gRPC::grpc++)
target_include_directories(chat_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate(TARGET chat_proto LANGUAGE cpp)
protobuf_generate(TARGET chat_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

#add_executable(chat_client chat_client.cpp)
#target_link_libraries(chat_client PRIVATE chat_proto SDBusCpp::sdbus-c++)

#add_executable(chat_service chat_service.cpp)
#target_link_libraries(chat_service PRIVATE chat_proto)

#install(TARGETS chat_client chat_service RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

## Test programs ##
add_executable(test_chat_client chat_client.cpp)
target_link_libraries(test_chat_client PRIVATE chat_proto SDBusCpp::sdbus-c++)
target_compile_definitions(test_chat_client PUBLIC -DUNIT_TEST)

add_executable(test_chat_service chat_service.cpp)
target_link_libraries(test_chat_service PRIVATE chat_proto)
target_compile_definitions(test_chat_service PUBLIC -DUNIT_TEST)

install(TARGETS test_chat_client test_chat_service RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/test")
