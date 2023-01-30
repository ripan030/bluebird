find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS person.proto)
add_executable(bb_pbuf main.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(bb_pbuf PRIVATE ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(bb_pbuf PRIVATE ${Protobuf_LIBRARIES})

install(TARGETS bb_pbuf RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
