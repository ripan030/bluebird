find_package(gRPC CONFIG REQUIRED)

add_executable(grpc_test grpc_test.cpp)
target_link_libraries(grpc_test PRIVATE gRPC::grpc++)
install(TARGETS grpc_test RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
