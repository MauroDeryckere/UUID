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

    for (size_t i = 0; i < 36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            continue; // skip dashes
        }
        REQUIRE(isHexDigit(buffer[i]));
    }

    printf("UUID: %s\n", buffer);
    std::cout << "UUID (using operator): " << uuid << "\n";
}

TEST_CASE("UUID hash has no collisions in large set", "[uuid][hash][collision]")
{
    size_t constexpr NUM_TESTS{ 10 };
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