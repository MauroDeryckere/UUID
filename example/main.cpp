#include <uuid.h>

#include <iostream>
#include <unordered_set>

int main()
{
	// --- Generate new UUIDs ---
	std::cout << "=== UUID Generation ===\n";

	MauUUID::UUID const id1{};
	MauUUID::UUID const id2{};

	std::cout << "UUID 1: " << id1 << "\n";
	std::cout << "UUID 2: " << id2 << "\n";
	std::cout << "Equal:  " << (id1 == id2 ? "yes" : "no") << "\n\n";

	// --- Parse from string ---
	std::cout << "=== Parsing ===\n";

	MauUUID::UUID const parsed{ "550e8400-e29b-41d4-a716-446655440000" };
	std::cout << "Parsed:  " << parsed << "\n";

	MauUUID::UUID const lenient{ MauUUID::UUID::FromStringLenient("{550e8400 e29b 41d4 a716 446655440000}") };
	std::cout << "Lenient: " << lenient << "\n";
	std::cout << "Match:   " << (parsed == lenient ? "yes" : "no") << "\n\n";

	// --- Null UUID ---
	std::cout << "=== Null UUID ===\n";

	std::cout << "null_uuid: " << MauUUID::null_uuid << "\n";
	std::cout << "IsNull:    " << (MauUUID::null_uuid.IsNull() ? "yes" : "no") << "\n";
	std::cout << "Bool:      " << (MauUUID::null_uuid ? "true" : "false") << "\n\n";

	// --- Comparison ---
	std::cout << "=== Comparison ===\n";

	auto const cmp{ id1 <=> id2 };
	std::cout << "id1 " << (cmp < 0 ? "<" : cmp > 0 ? ">" : "==") << " id2\n\n";

	// --- Hashing ---
	std::cout << "=== Hashing ===\n";

	std::unordered_set<MauUUID::UUID> set;
	for (int i{ 0 }; i < 1000; ++i)
	{
		set.emplace();
	}
	std::cout << "Generated " << set.size() << " unique UUIDs in unordered_set\n";

	return 0;
}
