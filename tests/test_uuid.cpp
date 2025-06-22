#include <catch2/catch_all.hpp>
#include "uuid.h"
#include <iostream>

#include <unordered_set>

TEST_CASE("UUID constructor generates non-zero data", "[uuid]")
{
    MauUUID::UUID uuid{};
    auto const& data = uuid.Data();

    REQUIRE(data.size() == 16);
    bool const allZero{ std::ranges::all_of(data, [](auto b) { return b == 0; }) };
    REQUIRE(!allZero);
}

TEST_CASE("UUID generates unique values", "[uuid][randomness]")
{
	size_t constexpr NUM_UUIDS{ 1'000'000 };

    std::vector<MauUUID::UUID> uuids{};
    uuids.reserve(NUM_UUIDS);

    for (size_t i{ 0 }; i < NUM_UUIDS; ++i)
    {
        uuids.emplace_back();
    }

    std::ranges::sort(uuids);
    auto duplicateIt{ std::ranges::adjacent_find(uuids) };
    REQUIRE(duplicateIt == uuids.end());
}

TEST_CASE("UUID CStr produces valid null-terminated string of length 36", "[uuid][cstr]")
{
    MauUUID::UUID uuid{};
    char buffer[37]{ };
    uuid.CStr(buffer);

    // Check null termination
    REQUIRE(buffer[36] == '\0');

    // Check length (should be 36 before null terminator)
    size_t const len{ std::strlen(buffer) };
    REQUIRE(len == 36);

    // Check the format (positions 8, 13, 18, 23 are dashes)
    REQUIRE(buffer[8] == '-');
    REQUIRE(buffer[13] == '-');
    REQUIRE(buffer[18] == '-');
    REQUIRE(buffer[23] == '-');

    auto isHexDigit{ [](char const c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'); } };

    // Skip dashes
    for (size_t i{ 0 }; i < 36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            continue;
        }
        REQUIRE(isHexDigit(buffer[i]));
    }

    printf("UUID: %s\n", buffer);
    std::cout << "UUID (using operator): " << uuid << "\n";
}

TEST_CASE("UUID hash has no collisions in large set", "[uuid][hash][collision]")
{
    size_t constexpr NUM_TESTS{ 1 };
    for (size_t i{ 0 }; i < NUM_TESTS; ++i)
    {
        size_t constexpr NUM_UUIDS{ 10'000'000 };
        std::unordered_set<MauUUID::UUID> uuidSet;
        uuidSet.reserve(NUM_UUIDS);

        for (size_t i{ 0 }; i < NUM_UUIDS; ++i)
        {
            uuidSet.emplace();
        }

        REQUIRE(uuidSet.size() == NUM_UUIDS);
    }
}

TEST_CASE("UUID Null behaves as expected", "[uuid][null]")
{
    // Construct a null UUID
    MauUUID::UUID nullUUID{ MauUUID::null_uuid };

    REQUIRE(nullUUID.IsNull());
    REQUIRE_FALSE(static_cast<bool>(nullUUID));

    auto const& data{ nullUUID.Data() };
    REQUIRE(std::ranges::all_of(data, [](auto b) { return b == 0; }));

    constexpr MauUUID::UUID constexprNull{ MauUUID::null_uuid };
    REQUIRE(nullUUID == constexprNull);
    REQUIRE_FALSE(nullUUID != nullUUID);
}

TEST_CASE("UUID FromStringFast parses valid UUID string without validation", "[uuid][parse][fast]")
{
    std::string_view constexpr validStr{ "123e4567-e89b-12d3-a456-426614174000" };

    // This should parse without throwing
    auto const uuid{ MauUUID::UUID::FromStringFast(validStr) };

    char buffer[37]{};
    uuid.CStr(buffer);

    REQUIRE(std::strcmp(buffer, validStr.data()) == 0);
}

TEST_CASE("UUID FromStringFast does not validate input (no throws, but output undefined)", "[uuid][parse][fast][unsafe]")
{
    std::string_view constexpr invalidStr{ "zzzzzzzz-zzzz-zzzz-zzzz-zzzzzzzzzzzz" };

    auto const uuid{ MauUUID::UUID::FromStringFast(invalidStr) };

    char buffer[37]{};
    uuid.CStr(buffer);

    REQUIRE(std::strlen(buffer) == 36);
    REQUIRE(std::strcmp(buffer, invalidStr.data()) != 0);
}

TEST_CASE("UUID constructor and FromStringFast produce identical UUID", "[uuid][parse][consistency]")
{
    std::string_view constexpr validStr{ "123e4567-e89b-12d3-a456-426614174000" };

    MauUUID::UUID const uuid1{ validStr };
    auto const uuid2{ MauUUID::UUID::FromStringFast(validStr) };

    REQUIRE(uuid1 == uuid2);
}

TEST_CASE("UUID parses from valid string", "[uuid][parse]")
{
    std::string const uuidStr{ "123e4567-e89b-12d3-a456-426614174000" };
    MauUUID::UUID const uuidFromStr{ uuidStr };

    char buffer[37]{};
    uuidFromStr.CStr(buffer);

    REQUIRE(std::strcmp(buffer, uuidStr.c_str()) == 0);
    REQUIRE(!uuidFromStr.IsNull());
}

TEST_CASE("UUID throws or fails on invalid string", "[uuid][parse][error]")
{
    std::string_view constexpr invalidStr{ "invalid-uuid-format" };
	REQUIRE_THROWS_AS(MauUUID::UUID{ invalidStr }, std::invalid_argument);
}

TEST_CASE("UUID Is valid string", "[uuid][parse]")
{
    using namespace MauUUID;
    REQUIRE(MauUUID::UUID::IsValidUUIDString("123e4567-e89b-12d3-a456-426655440000") == true);
    REQUIRE(MauUUID::UUID::IsValidUUIDString("00000000-0000-0000-0000-000000000000") == true);
    REQUIRE(MauUUID:: UUID::IsValidUUIDString("ffffffff-ffff-ffff-ffff-ffffffffffff") == true);
    REQUIRE(MauUUID::UUID::IsValidUUIDString("A1234567-E89B-12D3-A456-426655440000") == true); // uppercase hex

    // Invalid UUIDs
    REQUIRE(MauUUID::UUID::IsValidUUIDString("123e4567e89b12d3a456426655440000") == false); // missing dashes
    REQUIRE(MauUUID::UUID::IsValidUUIDString("123e4567-e89b-12d3-a456-42665544") == false); // too short
    REQUIRE(MauUUID::UUID::IsValidUUIDString("123e4567-e89b-12d3-a456-42665544000000") == false); // too long
    REQUIRE(MauUUID::UUID::IsValidUUIDString("123e4567-e89b-12d3-a456-42665544ZZZZ") == false); // invalid chars
    REQUIRE(MauUUID::UUID::IsValidUUIDString("123e4567-e89b-12d3-a456_426655440000") == false); // invalid dash replaced with underscore
    REQUIRE(MauUUID::UUID::IsValidUUIDString("") == false); // empty string
}

TEST_CASE("UUID extraction operator >> works correctly", "[uuid][stream]")
{
    using namespace MauUUID;


    SECTION("Extract valid UUID string")
    {
        std::istringstream iss("123e4567-e89b-12d3-a456-426655440000");
        MauUUID::UUID uuid;
        iss >> uuid;
        REQUIRE(iss);
        REQUIRE(uuid.Str() == "123e4567-e89b-12d3-a456-426655440000");
    }

    SECTION("Extract invalid UUID string causes stream failure")
    {
        std::istringstream iss("invalid-uuid-string-0000000000000000");
        MauUUID::UUID uuid;
        iss >> uuid;
        REQUIRE(!iss); // stream should be in fail state
    }

    SECTION("Extract empty string causes stream failure")
    {
        std::istringstream iss("");
        MauUUID::UUID uuid;
        iss >> uuid;
        REQUIRE(!iss);
    }

    SECTION("Extract UUID string with surrounding whitespace")
    {
        std::istringstream iss("  123e4567-e89b-12d3-a456-426655440000  ");
        MauUUID::UUID uuid;
        iss >> uuid;
        REQUIRE(iss);
        REQUIRE(uuid.Str() == "123e4567-e89b-12d3-a456-426655440000");
    }
}
