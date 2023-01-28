#### sdbus-cpp cmake build receipe ####

find_package(Threads REQUIRED)

set(SDBUSCPP_VERSION "0.8.3" CACHE STRING "sdbus-cpp version to build project")

message(STATUS "Building sdbus-cpp v${SDBUSCPP_VERSION}")

include(ExternalProject)

ExternalProject_Add(ep-sdbus-cpp
    PREFIX          sdbus-cpp-v${SDBUSCPP_VERSION}
    GIT_REPOSITORY  https://github.com/Kistler-Group/sdbus-cpp.git
    GIT_TAG         v${SDBUSCPP_VERSION}
    GIT_PROGRESS    TRUE
    GIT_SHALLOW     TRUE
    UPDATE_COMMAND  ""
    INSTALL_DIR     ${PROJECT_BINARY_DIRECTORY}
    CMAKE_ARGS      -DCMAKE_PREFIX_PATH:PATH=<INSTALL_DIR>
                    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
                    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DBUILD_SHARED_LIBS=OFF
                    -DBUILD_CODE_GEN=ON
                    -DBUILD_DOC=OFF
                    -DBUILD_TESTS=OFF
                    -DBUILD_LIBSYSTEMD=OFF
    DEPENDS         Systemd::Libsystemd
)

ExternalProject_Get_property(ep-sdbus-cpp SOURCE_DIR)
ExternalProject_Get_property(ep-sdbus-cpp BINARY_DIR)
ExternalProject_Get_property(ep-sdbus-cpp INSTALL_DIR)

add_library(SDBusCpp::sdbus-c++ STATIC IMPORTED GLOBAL)

file(MAKE_DIRECTORY ${INSTALL_DIR}/include)

set_target_properties(SDBusCpp::sdbus-c++ PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION ${INSTALL_DIR}/lib/libsdbus-c++.a
    INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include
)

set_property(TARGET SDBusCpp::sdbus-c++ APPEND PROPERTY
    INTERFACE_LINK_LIBRARIES Systemd::Libsystemd Threads::Threads
)
