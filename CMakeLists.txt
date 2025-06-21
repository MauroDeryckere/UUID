cmake_minimum_required(VERSION 3.15)
project(MAU_UUID VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(MAU_UUID INTERFACE)
target_include_directories(MAU_UUID INTERFACE include)

# Enable testing
enable_testing()

# Add test executable
add_executable(TEST_MAU_UUID tests/test_uuid.cpp)
target_link_libraries(TEST_MAU_UUID PRIVATE MAU_UUID)

add_test(NAME UUIDTest COMMAND TEST_MAU_UUID)
