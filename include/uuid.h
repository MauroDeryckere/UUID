#ifndef MAU_UUID
#define MAU_UUID

#include <array>
#include <cstring>
#include <cstdint>
#include <span>
#include <compare>

#ifdef _WIN32
#include <objbase.h>
#endif

#ifdef __linux__
#include <uuid/uuid.h>
#endif

#ifdef __APPLE__
#include <uuid/uuid.h>
#endif

#if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
#error Unsupported platform
#endif


namespace MauUUID
{
	class UUID final
	{
	public:
		constexpr UUID(std::array<uint8_t, 16> const& bytes) noexcept : m_Bytes{ bytes } {}

#ifdef _WIN32
		UUID() noexcept
		{
			GUID guid;
			CoCreateGuid(&guid);

			static_assert(sizeof(GUID) == 16, "GUID size mismatch");

			// Note: Windows stores GUID as little-endian for first 3 fields (RFC wants big-endian)
			m_Bytes[0] = static_cast<uint8_t>(guid.Data1 >> 24);
			m_Bytes[1] = static_cast<uint8_t>(guid.Data1 >> 16);
			m_Bytes[2] = static_cast<uint8_t>(guid.Data1 >> 8);
			m_Bytes[3] = static_cast<uint8_t>(guid.Data1);

			m_Bytes[4] = static_cast<uint8_t>(guid.Data2 >> 8);
			m_Bytes[5] = static_cast<uint8_t>(guid.Data2);

			m_Bytes[6] = static_cast<uint8_t>(guid.Data3 >> 8);
			m_Bytes[7] = static_cast<uint8_t>(guid.Data3);

			std::memcpy(&m_Bytes[8], guid.Data4, 8);
		}
#endif

#if defined(__linux__) || defined(__APPLE__)
		UUID() noexcept
		{
			uuid_generate(m_Bytes.data());
		}
#endif
		~UUID() = default;
		UUID(UUID const& other) noexcept = default;
		UUID(UUID&& other) noexcept = default;
		UUID& operator=(UUID const& other) noexcept = default;
		UUID& operator=(UUID&& other) noexcept = default;

		[[nodiscard]] bool IsNull() const noexcept
		{
			static std::array<uint8_t, 16> constexpr zeroBytes = { 0 };
			return m_Bytes == zeroBytes;
		}

		[[nodiscard]] std::array<uint8_t, 16> const& Data() const noexcept { return m_Bytes; }

		void CStr(std::span<char, 37> buffer) const noexcept
		{
			static constexpr char hex[] = "0123456789abcdef";

			// UUID layout: 8-4-4-4-12 hex digits with dashes
			// Format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx

			// bytes indices for each group
			// Group 1: bytes[0..3]
			// Group 2: bytes[4..5]
			// Group 3: bytes[6..7]
				
			// Group 4: bytes[8..9]
			// Group 5: bytes[10..15]

			const uint8_t* b = m_Bytes.data();
			char* out = buffer.data();

			// Helper to write two hex digits for one byte
			auto write_byte = [&](uint8_t byte) {
				*out++ = hex[byte >> 4];
				*out++ = hex[byte & 0x0F];
				};

			write_byte(b[0]); write_byte(b[1]); write_byte(b[2]); write_byte(b[3]);
			*out++ = '-';

			write_byte(b[4]); write_byte(b[5]);
			* out++ = '-';

			write_byte(b[6]); write_byte(b[7]);
			*out++ = '-';

			// These are always stored in network order
			write_byte(b[8]); write_byte(b[9]);
			*out++ = '-';

			write_byte(b[10]); write_byte(b[11]); write_byte(b[12]);
			write_byte(b[13]); write_byte(b[14]); write_byte(b[15]);

			// null-terminate
			*out = '\0';
		}

		[[nodiscard]] std::string Str() const noexcept
		{
			char buffer[37];
			CStr(std::span{ buffer });
			return { buffer };
		}

#pragma region operators
		[[nodiscard]] auto operator<=>(UUID const& other) const noexcept
		{
			return m_Bytes <=> other.m_Bytes;
		}
		[[nodiscard]] bool operator==(UUID const& other) const noexcept = default;

		[[nodiscard]] explicit operator bool() const noexcept
		{
			return !IsNull();
		}
#pragma endregion

	private:
		std::array<uint8_t, 16> m_Bytes;
	};

	constexpr UUID null_uuid{ std::array<uint8_t, 16>{} };

#pragma region operators
	inline std::ostream& operator<<(std::ostream& os, UUID const& uuid)
	{
		return os << uuid.Str();
	}
#pragma endregion

	struct UUIDHash final
	{
		size_t operator()(const UUID& uuid) const noexcept
		{
			auto const& bytes{ uuid.Data() };

			// Simple but decent hash: combine the bytes in chunks of size_t
			// Assuming size_t is at least 4 bytes; can be 8 on 64-bit
			size_t result{ 0 };

			size_t constexpr nChunks{ 16 / sizeof(size_t) };
			for (size_t i{ 0 }; i < nChunks; ++i)
			{
				size_t chunk{ 0 };
				// memcpy chunk bytes into size_t for alignment-safe conversion
				std::memcpy(&chunk, &bytes[i * sizeof(size_t)], sizeof(size_t));
				// boost::hash_combine
				result ^= chunk + 0x9e3779b9 + (result << 6) + (result >> 2);
			}

			return result;
		}
	};
}

#include <functional>
namespace std
{
	template <>
	struct hash<MauUUID::UUID>
	{
		size_t operator()(const MauUUID::UUID& uuid) const noexcept
		{
			return MauUUID::UUIDHash{}(uuid);
		}
	};
}

#endif