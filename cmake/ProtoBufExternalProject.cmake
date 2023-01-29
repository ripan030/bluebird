
set(PBUF_VERSION "21.12" CACHE STRING "ProtoBuf version to build project")

message(STATUS "Building ProtoBuf v${PBUF_VERSION}")

include(ExternalProject)

set(PBUF_PREFIX "protobuf-v${PBUF_VERSION}")
set(PBUF_DIR ${PROJECT_BINARY_DIR}/${PBUF_PREFIX})

ExternalProject_Add(ep-protobuf
    PREFIX          ${PBUF_PREFIX}
    GIT_REPOSITORY  https://github.com/google/protobuf.git
    GIT_TAG         v${PBUF_VERSION}
    GIT_PROGRESS    TRUE
    GIT_SHALLOW     TRUE
    UPDATE_DISCONNECTED 1
    INSTALL_DIR     ${PROJECT_BINARY_DIRECTORY}
    CMAKE_ARGS      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
                    -DCMAKE_INSTALL_PREFIX=${PBUF_DIR}
                    -Dprotobuf_BUILD_SHARED_LIBS=OFF
                    -Dprotobuf_BUILD_TESTS=OFF
                    -Dprotobuf_BUILD_EXAMPLES=OFF
                    -Dprotobuf_WITH_ZLIB=OFF
)

set(PBUF_INCLUDE_DIR ${PBUF_DIR}/include)
set(PBUF_LIBRARY ${PBUF_DIR}/lib/libprotobuf.a)
set(PBUF_PROTOC ${PBUF_DIR}/bin/protoc)

file(MAKE_DIRECTORY ${PBUF_INCLUDE_DIR})

#set(Protobuf_INCLUDE_DIR ${PBUF_DIR}/include)
#set(Protobuf_LIBRARY ${PBUF_DIR}/lib/libprotobuf.a)
#set(Protobuf_PROTOC_EXECUTABLE ${PBUF_DIR}/bin/protoc)

add_library(ProtoBufCPP::protobuf-c++ STATIC IMPORTED GLOBAL)

set_target_properties(ProtoBufCPP::protobuf-c++ PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION ${PBUF_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${PBUF_INCLUDE_DIR}
)
