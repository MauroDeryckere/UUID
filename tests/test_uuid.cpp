#include <catch2/catch_all.hpp>
#include "uuid.h"

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