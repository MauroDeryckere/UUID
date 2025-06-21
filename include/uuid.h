#ifndef MAU_UUID
#define MAU_UUID

#include <array>
#include <cstring>
#include <cstdint>

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

		[[nodiscard]] std::array<uint8_t, 16> const& Data() const noexcept { return m_Bytes; }
	private:
		std::array<uint8_t, 16> m_Bytes;
	};
}

#endif