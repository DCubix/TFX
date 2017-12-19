#include "String.h"
#include "Util.h"

#include <algorithm>

#include "SDL.h"
#include "Logger.h"

namespace tfx {
	String::String(const std::string& str) : m_str(str) {}

	String::String(char* str) : m_str(std::string(str)) {}

	String::String(const String& str) : m_str(str.m_str) {}

	String& String::operator =(const std::string& str) {
		m_str = str;
		return *this;
	}

	String& String::operator =(char* str) {
		m_str = std::string(str);
		return *this;
	}

	String& String::operator =(const String& str) {
		m_str = str.m_str;
		return *this;
	}

	String String::operator+(const String& str) {
		return String(m_str + str.m_str);
	}

	String String::subStr(u32 start, u32 len) {
		u32 l = len != -1 ? len : std::string::npos;
		return m_str.substr(start, l);
	}

	String String::replace(const String& what, const String& with) {
		String ret = (*this);
		size_t pos = 0;
		while ((pos = ret.m_str.find(what.m_str, pos)) != std::string::npos) {
			ret.m_str.replace(pos, what.length(), with.m_str);
			pos += with.length();
		}
		return ret;
	}

	i32 String::indexOf(const String& str) {
		return m_str.find_first_of(str.m_str);
	}

	i32 String::lastIndexOf(const String& str) {
		return m_str.find_last_of(str.m_str);
	}

	bool String::endsWith(const String& str) {
		if (str.length() > length()) return false;
		return std::equal(str.m_str.rbegin(), str.m_str.rend(), m_str.rbegin());
	}

	bool String::operator ==(char* str) {
		return m_str == str;
	}

	bool String::operator ==(const String& str) {
		return m_str == str.m_str;
	}

	const std::string Util::currentDateTime() {
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%m/%d/%Y %X", &tstruct);

		return buf;
	}

	const std::string Util::currentDateTimeNoFormat() {
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%m_%d_%Y_%H_%M_%S", &tstruct);

		return buf;
	}

	Vec<byte> Util::loadFile(const String& path) {
		std::ifstream file(path.str(), std::ios::ate | std::ios::binary);
		std::ifstream::pos_type pos = file.tellg();

		Vec<byte> result(pos);

		file.seekg(0, std::ios::beg);
		file.read((char*) &result[0], pos);

		file.close();

		LogInfo(String("Loaded file: ") + path);

		return result;
	}

	i32 Util::random(i32 min, i32 max) {
		return i32(random((float)min, (float)max));
	}

	float Util::random(float min, float max) {
		float r = (float) rand() / (float) RAND_MAX;
		return (min + r * (max - min));
	}

	float Util::toRadians(float degrees) {
		return degrees * 0.0174533f;
	}

	float Util::toDegrees(float radians) {
		return radians * 57.2958f;
	}

	double Util::getTime() {
		return double(SDL_GetTicks()) / 1000.0;
	}

}