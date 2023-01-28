add_executable(test_app main.c)
target_link_libraries(test_app PRIVATE event)
install(TARGETS test_app RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
