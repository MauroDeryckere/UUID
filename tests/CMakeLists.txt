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

include(CTest)
include(Catch)
catch_discover_tests(MauUUIDTests)
