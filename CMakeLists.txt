cmake_minimum_required(VERSION 3.5)
project(leveldb-grpc-server)
include(CTest)

file(GLOB SOURCES src/*.cpp)

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(protos)

option(LEVELDB_BUILD_GRPC_SERVER "Build LevelDB's GRPC Server" ON)
option(LEVELDB_BUILD_GRPC_CLIENT "Build LevelDB's GRPC Client" OFF)

if(LEVELDB_BUILD_GRPC_SERVER)
  add_subdirectory(third_party/leveldb)
  add_executable(${PROJECT_NAME} ${SOURCES})

  target_link_libraries(${PROJECT_NAME} leveldb protos)

  target_include_directories(${PROJECT_NAME}
	  PUBLIC
	      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	      $<INSTALL_INTERFACE:include>
          PRIVATE
      )

  # This makes the project importable from the build directory
  export(TARGETS ${PROJECT_NAME} FILE "${PROJECT_NAME}.cmake")
endif()


option(LEVELDB_BUILD_GRPC_SERVER_EXAMPLE "Build LevelDB's GRPC Server example" ON)

if(LEVELDB_BUILD_GRPC_SERVER_EXAMPLE OR LEVELDB_BUILD_GRPC_CLIENT)
  add_subdirectory(clients)
  if(LEVELDB_BUILD_GRPC_SERVER_EXAMPLE)
    add_subdirectory(examples)
  endif()
endif()
