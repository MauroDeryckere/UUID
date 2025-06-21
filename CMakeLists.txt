cmake_minimum_required(VERSION 3.15)
project(MAU_UUI)

option(MAU_UUID_BUILD_TESTS "Build tests for MAU_UUID" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(MAU_UUID INTERFACE)
target_include_directories(MAU_UUID INTERFACE include)

if(WIN32)
    # Windows - no need to link libuuid
elseif(UNIX)
    # Unix-like (Linux/macOS)
    target_link_libraries(TEST_MAU_UUID PRIVATE uuid)
endif()

if(MAU_UUID_BUILD_TESTS)

include(FetchContent)
    FetchContent_Declare(
      catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.4.0
    )

    FetchContent_MakeAvailable(catch2)

    enable_testing()

    add_executable(TEST_MAU_UUID tests/test_uuid.cpp)
    target_link_libraries(TEST_MAU_UUID PRIVATE MAU_UUID Catch2::Catch2WithMain)

    include(CTest)
    include(Catch)
    catch_discover_tests(TEST_MAU_UUID)

    add_test(NAME UUIDTest COMMAND TEST_MAU_UUID)
endif()
