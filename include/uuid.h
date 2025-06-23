#ifndef MAU_UUID_H
#define MAU_UUID_H

#include <array>
#include <cstring>
#include <cstdint>
#include <span>
#include <compare>

#ifdef _WIN32
#define NOMINMAX
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
	static std::array<uint8_t, 256> constexpr CreateHexLUT()
	{
		std::array<uint8_t, 256> lut{};
		lut.fill(0xFF);

		for (char c{ '0' }; c <= '9'; ++c)
		{
			lut[static_cast<uint8_t>(c)] = c - '0';
		}
		for (char c{ 'a' }; c <= 'f'; ++c)
		{
			lut[static_cast<uint8_t>(c)] = c - 'a' + 10;
		}
		for (char c{ 'A' }; c <= 'F'; ++c)
		{
			lut[static_cast<uint8_t>(c)] = c - 'A' + 10;
		}

		return lut;
	}

	static size_t constexpr HEX_PAIRS[16][2]
	{
	{0,1}, {2,3}, {4,5}, {6,7},
	{9,10}, {11,12},
	{14,15}, {16,17},
	{19,20}, {21,22},
	{24,25}, {26,27},
	{28,29}, {30,31},
	{32,33}, {34,35}
	};

	static constexpr std::array<uint8_t, 256> HEX_LUT{ CreateHexLUT() };

	class UUID final
	{
	public:
		/**
		 * @brief Initialize a UUID with bytes.
		 * @param bytes Bytes to initialize the UUID with.
		 */
		constexpr explicit UUID(std::array<uint8_t, 16> const& bytes) noexcept : m_Bytes{ bytes } { }
		/**
		 * @brief Initialize a UUID with bytes (32 bit variation).
		 * @param bytes Bytes to initialize the UUID with.
		 */
		explicit UUID(std::array<uint32_t, 4> const& data32) noexcept { std::memcpy(m_Bytes.data(), data32.data(), sizeof(m_Bytes)); }
		/**
		 * @brief Initialize a UUID with bytes (64 bit variation).
		 * @param bytes Bytes to initialize the UUID with.
		 */
		explicit UUID(std::array<uint64_t, 2> const& data64) noexcept { std::memcpy(m_Bytes.data(), data64.data(), sizeof(m_Bytes)); }

		/**
		 * @brief Initialize a UUID with a string.
		 * @param str String to initialize the UUID with.
		 * @warning The string must be in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" (36 characters including dashes), does assert for a valid format in Debug.
		 */
		explicit UUID(std::string_view const str) { *this = FromString(str); }

	#ifdef _WIN32
		/**
		 * @brief Generate a new UUID.
		 */
		UUID() noexcept
		{
			GUID guid;

			#ifdef _DEBUG
				auto const result{ CoCreateGuid(&guid) };
				assert(result == SEC_E_OK);
			#else
				CoCreateGuid(&guid);
			#endif

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
		/**
		 * @brief Generate a new UUID.
		 */
		UUID() noexcept
		{
			uuid_generate(m_Bytes.data());
		}
	#endif

		~UUID() = default;
		UUID(UUID const&) noexcept = default;
		UUID(UUID&&) noexcept = default;
		UUID& operator=(UUID const&) noexcept = default;
		UUID& operator=(UUID&&) noexcept = default;

		/**
		 * @brief Check if a UUID is null (all bytes are zero).
		 * @return True if the UUID is null, false otherwise.
		 */
		[[nodiscard]] bool IsNull() const noexcept
		{
			static std::array<uint8_t, 16> constexpr zeroBytes{ 0 };
			return m_Bytes == zeroBytes;
		}
		/**
		 * @brief Get the raw byte data of the UUID.
		 * @return Reference (const) to the internal byte array.
		 */
		[[nodiscard]] std::array<uint8_t, 16> const& Data() const noexcept { return m_Bytes; }
		/**
		 * @brief Get the raw byte data of the UUID (32).
		 * @return Copy of the internal byte array in 32 bit format.
		 */
		[[nodiscard]] std::array<uint32_t, 4> Data32() const noexcept
		{
			std::array<uint32_t, 4> result{};
			std::memcpy(result.data(), m_Bytes.data(), sizeof(m_Bytes));
			return result;
		}
		/**
		 * @brief Get the raw byte data of the UUID (64).
		 * @return Copy of the internal byte array in 64 bit format.
		 */
		[[nodiscard]] std::array<uint64_t, 2> Data64() const noexcept
		{
			std::array<uint64_t, 2> result{};
			std::memcpy(result.data(), m_Bytes.data(), sizeof(m_Bytes));
			return result;
		}

#pragma region Strings
		/**
		 * @brief Get string representation of the UUID in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
		 * @param buffer Buffer to write the string representation to, must be at least 37 characters long (36 for UUID + 1 for null terminator).
		 */
		void CStr(std::span<char, 37> buffer) const noexcept
		{
			static char constexpr hex[]{ "0123456789abcdef" };

			// UUID layout: 8-4-4-4-12 hex digits with dashes
			// Format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx

			// bytes indices for each group
			// Group 1: bytes[0..3]
			// Group 2: bytes[4..5]
			// Group 3: bytes[6..7]
				
			// Group 4: bytes[8..9]
			// Group 5: bytes[10..15]

			uint8_t const* b{ m_Bytes.data() };
			char* out{ buffer.data() };

			// Helper to write two hex digits for one byte
			auto const write_byte{ [&](uint8_t const byte)
				{
					*out++ = hex[byte >> 4];
					*out++ = hex[byte & 0x0F];
				} };

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
		/**
		 * @brief Get string representation of the UUID in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
		 * @return String representation of the UUID.
		 */
		[[nodiscard]] std::string Str() const noexcept
		{
			char buffer[37];
			CStr(std::span{ buffer });
			return { buffer };
		}
		/**
		 * @brief Create a UUID from a string in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
		 * @param str String to parse into a UUID.
		 * @warning The string must be in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" (36 characters including dashes), does assert for a valid format in Debug.
		 * @return UUID object created from the string.
		 */
		[[nodiscard]] static UUID FromString(std::string_view const str) noexcept
		{
			assert(IsValidString(str) && "Invalid UUID format!");

			UUID uuid;

			for (size_t i{ 0 }; i < 16; ++i)
			{
				auto [hi, lo] = HEX_PAIRS[i];
				uuid.m_Bytes[i] = (HEX_LUT[static_cast<uint8_t>(str[hi])] << 4) | HEX_LUT[static_cast<uint8_t>(str[lo])];
			}

			return uuid;
		}
		/**
		 * @brief Create a UUID from a string in a lenient format, ignoring dashes, spaces, and curly braces.
		 * @param str String to parse into a UUID.
		 * @warning The string must contain exactly 32 hexadecimal characters, does assert for a valid format in Debug.
		 * @return UUID object created from the string.
		 */
		[[nodiscard]] static UUID FromStringLenient(std::string_view const str) noexcept
		{
			assert(IsValidStringLenient(str));

			std::string cleaned;
			cleaned.reserve(32);

			for (char const c : str)
			{
				if (c == '{' || c == '}' || c == '-' || c == ' ') continue;
				cleaned.push_back(c);
			}

			UUID uuid;
			for (size_t i{ 0 }; i < 16; ++i)
			{
				uint8_t const hi{ HEX_LUT[static_cast<uint8_t>(cleaned[i * 2])] };
				uint8_t const lo{ HEX_LUT[static_cast<uint8_t>(cleaned[i * 2 + 1])] };
				uuid.m_Bytes[i] = (hi << 4) | lo;
			}

			return uuid;
		}
		/**
		 * @brief Try to create a UUID from a string in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
		 * @param str String to parse into a UUID.
		 * @param out Output parameter to store the created UUID if parsing is successful.
		 * @return True if parsing was successful, false otherwise.
		 */
		[[nodiscard]] static bool TryParse(std::string_view const str, UUID& out) noexcept
		{
			if (!IsValidString(str))
			{
				return false;
			}
			out = FromString(str);
			return true;
		}
		/**
		 * @brief Try to create a UUID from a string in a lenient format, ignoring dashes, spaces, and curly braces.
		 * @param str String to parse into a UUID.
		 * @param out Output parameter to store the created UUID if parsing is successful.
		 * @return True if parsing was successful, false otherwise.
		 */
		[[nodiscard]] static bool TryParseLenient(std::string_view const str, UUID& out) noexcept
		{
			if (!IsValidStringLenient(str))
			{
				return false;
			}

			out = FromStringLenient(str);
			return true;
		}
		/**
		 * @brief Check if a string is a valid UUID in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
		 * @param str String to check.
		 * @return True if the string is a valid UUID, false otherwise.
		 */
		[[nodiscard]] static bool IsValidString(std::string_view const str) noexcept
		{
			if (str.size() != 36 || str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') return false;

			for (size_t i{ 0 }; i < 36; ++i)
			{
				if (i == 8 || i == 13 || i == 18 || i == 23) continue;
				if (MauUUID::HEX_LUT[static_cast<uint8_t>(str[i])] == 0xFF) return false;
			}
			return true;
		}
		/**
		 * @brief Check if a string is a valid UUID in a lenient format, ignoring dashes, spaces, and curly braces.
		 * @param str String to check.
		 * @return True if the string is a valid UUID, false otherwise.
		 */
		[[nodiscard]] static bool IsValidStringLenient(std::string_view const str) noexcept
		{
			std::size_t count{ 0 };
			for (char const c : str)
			{
				if (c == '{' || c == '}' || c == '-' || c == ' ') continue;
				if (HEX_LUT[static_cast<uint8_t>(c)] == 0xFF) return false;

				++count;
			}
			return count == 32;
		}
#pragma endregion
#pragma region operators
		[[nodiscard]] auto operator<=>(UUID const& other) const noexcept
		{
			return m_Bytes <=> other.m_Bytes;
		}
		[[nodiscard]] bool operator==(UUID const&) const noexcept = default;

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
	inline std::istream& operator>>(std::istream& is, MauUUID::UUID& uuid)
	{
		std::string str;
		// read whitespace-delimited token from stream
		is >> str;

		if (not UUID::TryParse(str, uuid))
		{
			is.setstate(std::ios::failbit);
		}

		return is;
	}

#pragma endregion

	// Chunk-based hash (in size_t chunks)
	struct UUIDHashChunks final
	{
		size_t operator()(const UUID& uuid) const noexcept
		{
			auto const& bytes{ uuid.Data() };

			// Simple but decent hash: combine the bytes in chunks of size_t
			// Assuming size_t is at least 4 bytes; can be 8 on 64-bit
			size_t result{ 0 };

			size_t constexpr nChunks{ 16 / sizeof(size_t) };
			static_assert(16 % sizeof(size_t) == 0);

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

	// Byte-by-byte hash
	struct UUIDHashBytePerByte final
	{
		size_t operator()(const UUID& uuid) const noexcept
		{
			auto const& bytes{ uuid.Data() };
			size_t result{ 0 };
			for (size_t i{ 0 }; i < bytes.size(); ++i)
			{
				result ^= static_cast<size_t>(bytes[i]) + 0x9e3779b9 + (result << 6) + (result >> 2);
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
			return MauUUID::UUIDHashChunks{}(uuid);
		}
	};
}

#endif