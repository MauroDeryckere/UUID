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
#error Unsupported platform, may be supported if you enable std UUID
#endif


namespace MauUUID
{
	class UUID final
	{
	public:
#ifdef _WIN32
		UUID()
		{
			GUID guid;
			CoCreateGuid(&guid);

			static_assert(sizeof(GUID) == 16, "GUID size mismatch");
			memcpy(m_Bytes.data(), &guid, 16);
		}
#endif

#if defined(__linux__) || defined(__APPLE__)
		UUID()
		{
			uuid_generate(m_Bytes.data());
		}
#endif
		~UUID() = default;
		UUID(UUID const& other) noexcept = default;
		UUID(UUID&& other) noexcept = default;
		UUID& operator=(UUID const& other) noexcept = default;
		UUID& operator=(UUID&& other) noexcept = default;


		[[nodiscard]] std::array<uint8_t, 16> const& Data() const noexcept { return m_Bytes; }
		void CStr(std::span<char, 37> buffer) const noexcept
		{
			//TODO
			//std::snprintf(buffer.data(), buffer.size(),
			//	"%02x%02x%02x%02x-"
			//	"%02x%02x-"
			//	"%02x%02x-"
			//	"%02x%02x-"
			//	"%02x%02x%02x%02x%02x%02x",
			//	m_Bytes[0], m_Bytes[1], m_Bytes[2], m_Bytes[3],
			//	m_Bytes[4], m_Bytes[5],
			//	m_Bytes[6], m_Bytes[7],
			//	m_Bytes[8], m_Bytes[9],
			//	m_Bytes[10], m_Bytes[11], m_Bytes[12], m_Bytes[13], m_Bytes[14], m_Bytes[15]);
		}

		[[nodiscard]] std::string Str() const noexcept
		{
			//TODO
			return {};
		}

#pragma region operators
		[[nodiscard]] auto operator<=>(UUID const& other) const noexcept
		{
			return m_Bytes <=> other.m_Bytes;
		}
		[[nodiscard]] bool operator==(UUID const& other) const noexcept = default;
#pragma endregion
	private:
		std::array<uint8_t, 16> m_Bytes;
	};
}

#endif