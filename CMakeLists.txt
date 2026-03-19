cmake_minimum_required(VERSION 3.16)
project(MauUUID
	VERSION 1.0.0
	LANGUAGES CXX
	DESCRIPTION "Lightweight, cross-platform C++20 UUID library"
)

# --- Options ---
option(MAU_UUID_BUILD_EXAMPLE "Build the example application" OFF)
option(MAU_UUID_BUILD_TESTS   "Build the test suite"          OFF)

# --- Library (header-only) ---
add_library(MauUUID INTERFACE)
add_library(MauUUID::MauUUID ALIAS MauUUID)

target_include_directories(MauUUID
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

target_compile_features(MauUUID INTERFACE cxx_std_20)

# --- Platform dependencies ---
if(WIN32)
	target_link_libraries(MauUUID INTERFACE ole32)
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
	target_link_libraries(MauUUID INTERFACE uuid)
endif()

# --- Example ---
if(MAU_UUID_BUILD_EXAMPLE)
	add_subdirectory(example)
endif()

# --- Tests ---
if(MAU_UUID_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

# --- Install ---
include(GNUInstallDirs)

install(TARGETS MauUUID
	EXPORT MauUUIDTargets
)

install(DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT MauUUIDTargets
	FILE MauUUIDTargets.cmake
	NAMESPACE MauUUID::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MauUUID
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/MauUUIDConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/MauUUIDConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MauUUID
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/MauUUIDConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/MauUUIDConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/MauUUIDConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MauUUID
)
