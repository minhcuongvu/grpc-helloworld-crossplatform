# CMakeList.txt
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif (WIN32)

project(grpc_example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Find gRPC and ProtoBuf packages
if (WIN32)
    message(STATUS "WIN32")
    # set(Protobuf_INCLUDE_DIRS "C:/Dev/vcpkg/installed/x64-windows/include")
    # set(Protobuf_LIBRARIES "optimized;C:/Dev/vcpkg/installed/x64-windows/bin/libprotobuf.dll;debug;C:/Dev/vcpkg/installed/x64-windows/debug/bin/libprotobufd.dll")
    # set(Protobuf_PROTOC_EXECUTABLE "C:/Dev/vcpkg/installed/x64-windows/tools/protobuf/protoc.exe")
    set(gRPC_INCLUDE_DIRS "C:/Dev/vcpkg/installed/x64-windows/include")
elseif (UNIX)
    message(STATUS "UNIX")
    set(HOME_DIR $ENV{HOME})
    set(Protobuf_INCLUDE_DIRS "${HOME_DIR}/.local/include/google/protobuf/")
    set(Protobuf_LIBRARIES "${HOME_DIR}/.local/lib/")
    set(Protobuf_PROTOC_EXECUTABLE "${HOME_DIR}/.local/bin/protoc")
    set(gRPC_ROOT "${HOME_DIR}/.local/")
    set(gRPC_INCLUDE_DIRS "${HOME_DIR}/.local/include/grpgc/")
endif()

find_package( Protobuf CONFIG REQUIRED )
find_package( gRPC CONFIG REQUIRED )
message(STATUS "=====")
message(STATUS "Using absl version ${absl_VERSION}")
message(STATUS "=====")
message(STATUS "Using Protobuf version ${Protobuf_VERSION}")
message(STATUS "Protobuf Include Directory: ${Protobuf_INCLUDE_DIRS}")
message(STATUS "Protobuf Library: ${Protobuf_LIBRARIES}")
message(STATUS "Protobuf Protoc Executable: ${Protobuf_PROTOC_EXECUTABLE}")
message(STATUS "=====")
message(STATUS "Using gRPC version ${gRPC_VERSION}")
message(STATUS "gRPC Include Directory: ${gRPC_INCLUDE_DIRS}")
message(STATUS "=====")

# Output directories
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Define directory for generated ProtoBuf and gRPC files
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Setup custom library for using ProtoBuf and gRPC
set(PROTO_FILE ${CMAKE_SOURCE_DIR}/proto/helloworld.proto)
add_library(protolib ${PROTO_FILE})
target_link_libraries(protolib gRPC::grpc++ gRPC::grpc++_reflection gRPC::gpr gRPC::grpc gRPC::grpc++ protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
target_include_directories(protolib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Detect the operating system and set the gRPC plugin location
if (WIN32)
    # Get the location of grpc_cpp_plugin on Windows using the gRPC target
    # set(gRPC_INCLUDE_DIRS "C:/Dev/vcpkg/installed/x64-windows/tools/grpc/grpc_cpp_plugin.exe")
    get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
elseif (UNIX)
    # Set the grpc_cpp_plugin_location to the specified path on Unix/Linux
    set(grpc_cpp_plugin_location "/home/adrian/.local/bin/grpc_cpp_plugin")
    
    # Ensure the plugin is executable
    if (NOT EXISTS "${grpc_cpp_plugin_location}")
        message(FATAL_ERROR "gRPC C++ plugin not found at ${grpc_cpp_plugin_location}")
    endif()
endif()
message(STATUS "gRPC C++ plugin location: ${grpc_cpp_plugin_location}")

# Generate ProtoBuf code
protobuf_generate(TARGET protolib LANGUAGE cpp
PROTOC_OUT_DIR "${GENERATED_DIR}")
protobuf_generate(TARGET protolib LANGUAGE grpc 
GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc 
PROTOC_OUT_DIR "${GENERATED_DIR}"
PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

# Include the generated code directory
include_directories(${GENERATED_DIR})

# Target executables
add_executable(server ${CMAKE_SOURCE_DIR}/src/server.cpp)
add_executable(client ${CMAKE_SOURCE_DIR}/src/client.cpp)

# Link to our custom library
target_link_libraries(server protolib)
target_link_libraries(client protolib)

# Set the output directories for executables
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
