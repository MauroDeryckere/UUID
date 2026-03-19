include(FetchContent)
FetchContent_Declare(
	catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(catch2)

add_executable(MauUUIDTests test_uuid.cpp)
target_link_libraries(MauUUIDTests PRIVATE MauUUID::MauUUID Catch2::Catch2WithMain)

target_compile_features(MauUUIDTests PRIVATE cxx_std_20)

# --- Link-time optimization ---
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
	set_target_properties(MauUUIDTests PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# --- Warnings as errors ---
target_compile_options(MauUUIDTests PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
	set_target_properties(MauUUIDTests PROPERTIES
		MACOSX_BUNDLE TRUE
		XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.mauuuid.tests"
	)
endif()

# --- Portable fallback tests (same tests, forced to use std::random) ---
if(NOT MAU_UUID_USE_RANDOM)
	add_executable(MauUUIDTestsPortable test_uuid.cpp)
	target_link_libraries(MauUUIDTestsPortable PRIVATE MauUUID::MauUUID Catch2::Catch2WithMain)
	target_compile_features(MauUUIDTestsPortable PRIVATE cxx_std_20)
	target_compile_definitions(MauUUIDTestsPortable PRIVATE MAU_UUID_USE_RANDOM)

	target_compile_options(MauUUIDTestsPortable PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
	)

	if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
		set_target_properties(MauUUIDTestsPortable PROPERTIES
			MACOSX_BUNDLE TRUE
			XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.mauuuid.tests.portable"
		)
	endif()
endif()

if(NOT CMAKE_CROSSCOMPILING)
	include(CTest)
	include(Catch)
	catch_discover_tests(MauUUIDTests)

	if(NOT MAU_UUID_USE_RANDOM)
		catch_discover_tests(MauUUIDTestsPortable)
	endif()
endif()
