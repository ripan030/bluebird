cmake_minimum_required(VERSION 3.13)

project(game VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

#add_compile_options(-Wall -Werror -g)

find_package(SFML COMPONENTS system window graphics network audio REQUIRED)

add_executable(test test.cpp EventMgr.cpp Window.cpp StateMgr.cpp StateIntro.cpp MainMenu.cpp GamePlay.cpp GamePaused)

target_link_libraries(test PRIVATE sfml-graphics)
target_include_directories(test PRIVATE ./)
