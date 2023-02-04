include(ExternalProject)

message(STATUS "EP CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "EP PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message(STATUS "EP PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message(STATUS "EP CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "EP CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")

set(PBUF_DIR ${PROJECT_BINARY_DIR}/protobuf)

ExternalProject_Add(ep-protobuf
    PREFIX              protobuf
    SOURCE_DIR          "${CMAKE_CURRENT_SOURCE_DIR}/protobuf/cmake"
    CMAKE_CACHE_ARGS    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
                        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
                        -DCMAKE_INSTALL_PREFIX:PATH=${PBUF_DIR}
                        -Dprotobuf_BUILD_SHARED_LIBS:BOOL=OFF
                        -Dprotobuf_BUILD_TESTS:BOOL=OFF
                        -Dprotobuf_BUILD_EXAMPLES:BOOL=OFF
                        -Dprotobuf_WITH_ZLIB:BOOL=OFF
)

set(Protobuf_INCLUDE_DIR ${PBUF_DIR}/include)
set(Protobuf_LIBRARY ${PBUF_DIR}/lib/libprotobuf.a)
set(Protobuf_PROTOC_EXECUTABLE ${PBUF_DIR}/bin/protoc)

# Following submodule update --init required in the grpc dir.
set(GRPC_GIT_SUBMODULES
    third_party/zlib
    third_party/re2
    third_party/abseil-cpp
    third_party/boringssl
    third_party/cares/cares
)

ExternalProject_Add(ep-grpc
    PREFIX              grpc
    SOURCE_DIR          "${CMAKE_CURRENT_SOURCE_DIR}/grpc"
    CMAKE_CACHE_ARGS    -DgRPC_INSTALL:BOOL=ON
                        -DgRPC_BUILD_TESTS:BOOL=OFF
                        -DgRPC_PROTOBUF_PROVIDER:STRING=package
                        -DgRPC_PROTOBUF_PACKAGE_TYPE:STRING=CONFIG
                        -DgRPC_CARES_PROVIDER:STRING=module
                        -DProtobuf_DIR:PATH=${PROJECT_BINARY_DIR}/protobuf/lib/cmake/protobuf
                        -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/grpc

    DEPENDS             ep-protobuf
)

ExternalProject_Add(ep-bluebird
    PREFIX              bluebird
    SOURCE_DIR          "${PROJECT_SOURCE_DIR}/src"
    BUILD_ALWAYS        TRUE
    CMAKE_CACHE_ARGS    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
                        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
                        -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/bluebird
                        -DProtobuf_INCLUDE_DIR:PATH=${Protobuf_INCLUDE_DIR}
                        -DProtobuf_LIBRARY:PATH=${Protobuf_LIBRARY}
                        -DProtobuf_PROTOC_EXECUTABLE:PATH=${Protobuf_PROTOC_EXECUTABLE}
                        -DProtobuf_DIR:PATH=${PROJECT_BINARY_DIR}/protobuf/lib/cmake/protobuf
                        -DgRPC_DIR:PATH=${PROJECT_BINARY_DIR}/grpc/lib/cmake/grpc
                        -Dabsl_DIR:PATH=${PROJECT_BINARY_DIR}/grpc/lib/cmake/absl
    DEPENDS             ep-protobuf ep-grpc SDBusCpp::sdbus-c++
)
