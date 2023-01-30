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

ExternalProject_Add(ep-bluebird
    PREFIX              bluebird
    SOURCE_DIR          "${PROJECT_SOURCE_DIR}/src"
    CMAKE_CACHE_ARGS    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
                        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
                        -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/bluebird
                        -DProtobuf_INCLUDE_DIR:PATH=${Protobuf_INCLUDE_DIR}
                        -DProtobuf_LIBRARY:PATH=${Protobuf_LIBRARY}
                        -DProtobuf_PROTOC_EXECUTABLE:PATH=${Protobuf_PROTOC_EXECUTABLE}
    DEPENDS             ep-protobuf SDBusCpp::sdbus-c++
)
