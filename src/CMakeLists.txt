cmake_minimum_required(VERSION 3.13)

project(ep-bluebird)

include(GNUInstallDirs)

include_directories(lib/include)
add_subdirectory(lib)
add_subdirectory(app)
