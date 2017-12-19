#ifndef TFX_STD_STRING_H
#define TFX_STD_STRING_H

#include <string>
#include <map>
#include <vector>
#include <array>
#include <ostream>
#include <cstdint>
#include <sstream>
#include <fstream>
#include <time.h>

#define SafeDelete(x) if (x != nullptr) { delete x; x = nullptr; }
#define Unsafe(x) if (x != nullptr)

#if defined(DEBUG) || defined(_DEBUG)
#	define TFX_DEBUG
#endif

#include "tinypng.h"

namespace tfx {
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using byte = uint8_t;

	class String {
	public:
		String() {}
		String(const std::string& str);
		String(char* str);
		String(const String& str);

		template <typename T>
		String(const T& t) {
			std::stringstream stm;
			stm << t;
			m_str = stm.str();
		}
		
		String& operator =(const std::string& str);
		String& operator =(char* str);
		String& operator =(const String& str);

		template <typename T>
		String& operator =(const T& t) {
			std::stringstream stm;
			stm << t;
			m_str = stm.str();
			return *this;
		}

		String operator +(const String& str);

		String subStr(u32 start, u32 len = -1);
		String replace(const String& what, const String& with);
		i32 indexOf(const String& str);
		i32 lastIndexOf(const String& str);
		bool endsWith(const String& str);

		bool operator ==(char* str);
		bool operator ==(const String& str);

		operator std::string() const { return m_str; }
		operator const char*() const { return m_str.c_str(); }

		const std::string& str() const { return m_str; }
		u32 length() const { return m_str.length(); }

		/*friend std::ostream& operator<< (std::ostream& stream, const String& obj) {
			stream << obj.m_str;
			return stream;
		}*/
	private:
		std::string m_str;
	};

	template <typename T> using Vec = std::vector<T>;
	template <typename K, typename V> using Map = std::map<K, V>;
	template <typename T, u32 size> using Array = std::array<T, size>;

	class Util {
	public:
		static const std::string currentDateTime();
		static const std::string currentDateTimeNoFormat();
		static Vec<byte> loadFile(const String& path);
		static i32 random(i32 min, i32 max);
		static float random(float min, float max);
		static float toRadians(float degrees);
		static float toDegrees(float radians);
		static double getTime();
	};
}

#endif // TFX_STD_STRING_H