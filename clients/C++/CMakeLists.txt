cmake_minimum_required(VERSION 3.5)
project(leveldb-grpc-client)

file(GLOB SOURCES src/*.cpp)

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} proto-types)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	    $<INSTALL_INTERFACE:include>
        PRIVATE
    )

# This makes the project importable from the build directory
export(TARGETS ${PROJECT_NAME} FILE "${PROJECT_NAME}.cmake")
