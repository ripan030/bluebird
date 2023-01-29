cmake_minimum_required(VERSION 3.13)

project(bluebird VERSION 0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror -g)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)
pkg_check_modules(Systemd IMPORTED_TARGET GLOBAL libsystemd>=236)
if(NOT TARGET PkgConfig::Systemd)
    message(FATAL_ERROR "system libsystemd not found")
endif()
message(STATUS "Bulding sdbus cpp with system libsystemd")
add_library(Systemd::Libsystemd ALIAS PkgConfig::Systemd)

include(${PROJECT_SOURCE_DIR}/cmake/SdbusCppExternalProject.cmake)

include(${PROJECT_SOURCE_DIR}/cmake/ProtoBufExternalProject.cmake)

add_subdirectory(src)

#### Install ####
include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE PATH "..." FORCE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX : ${CMAKE_INSTALL_PREFIX}")
