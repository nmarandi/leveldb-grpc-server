cmake_minimum_required(VERSION 3.5)
project(proto-types VERSION 1.0 LANGUAGES CXX)
find_package(Protobuf)
include_directories(${PROTOBUF_INCLUDE_DIRS})

set(MODULE_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads)
file(GLOB PROTO_FILES *.proto)

set(PROTOS_SRC "")
set(PPTH --proto_path=${CMAKE_CURRENT_SOURCE_DIR})

find_package(gRPC)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin) # Get full path to plugin
message("${GRPC_CPP_PLUGIN}")

function(PROTOBUF_GENERATE_GRPC_CPP SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_GRPC_CPP() called without any proto files")
    return()
  endif()

  if(PROTOBUF_GENERATE_CPP_APPEND_PATH) # This variable is common for all types of output.
    # Create an include path for each file specified
    foreach(FIL ${ARGN})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  else()
    set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(DEFINED PROTOBUF_IMPORT_DIRS)
    foreach(DIR ${Protobuf_IMPORT_DIRS})
      get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  endif()

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
      COMMAND  ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
           --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
           ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL} ${Protobuf_PROTOC_EXECUTABLE}
      COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
      VERBATIM)
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

PROTOBUF_GENERATE_GRPC_CPP(PROTOS_SRC PROTOS_HDRS ${PROTO_FILES})
foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(ABSOLUTE_NAME ${PROTO_FILE} ABSOLUTE)
    get_filename_component(FILE_NAME ${PROTO_FILE} NAME_WE)
    list(APPEND PROTOS_SRC "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.pb.cc")
    list(APPEND PROTOS_HDRS "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.pb.h")
    protobuf_generate_cpp(TARGET ${MODULE_NAME} PROTOS ${FILE_NAME}.proto)
endforeach(PROTO_FILE ${PROTO_FILES})

file(GLOB SRCS *.cc)

add_library(${MODULE_NAME} ${PROTOS_SRC} ${SRCS})
target_link_libraries(${MODULE_NAME} ${PROTOBUF_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} grpc++_unsecure grpc++)


target_include_directories(${MODULE_NAME}
	PUBLIC
	    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
	    $<INSTALL_INTERFACE:include>
    )

# This makes the project importable from the build directory
export(TARGETS ${MODULE_NAME} FILE "${MODULE_NAME}.cmake")
