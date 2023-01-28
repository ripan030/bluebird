find_package(sdbus-c++ REQUIRED)

add_executable(sdbus_server sdbus_server.cpp)
target_link_libraries(sdbus_server PRIVATE SDBusCpp::sdbus-c++)

add_executable(sdbus_client sdbus_client.cpp)
target_link_libraries(sdbus_client PRIVATE SDBusCpp::sdbus-c++)

install(TARGETS sdbus_server sdbus_client RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
