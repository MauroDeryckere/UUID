add_executable(MauUUIDExample main.cpp)
target_link_libraries(MauUUIDExample PRIVATE MauUUID::MauUUID)

target_compile_features(MauUUIDExample PRIVATE cxx_std_20)

# --- Link-time optimization ---
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
	set_target_properties(MauUUIDExample PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# --- Warnings as errors ---
target_compile_options(MauUUIDExample PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

set_target_properties(MauUUIDExample PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
	set_target_properties(MauUUIDExample PROPERTIES
		MACOSX_BUNDLE TRUE
		XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.mauuuid.example"
	)
endif()
