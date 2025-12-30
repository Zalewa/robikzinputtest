#pragma once

#include <algorithm>
#include <string>

namespace robikzinputtest::text {
inline std::string ltrim(std::string s) {
	s.erase(
		s.begin(),
		std::find_if(
			s.begin(), s.end(),
			[](unsigned char ch) { return !std::isspace(ch); }
		)
	);
	return s;
}

inline std::string rtrim(std::string s) {
	s.erase(
		std::find_if(
			s.rbegin(), s.rend(),
			[](unsigned char ch) { return !std::isspace(ch); }
		).base(),
		s.end()
	);
	return s;
}

inline std::string trim(std::string s) {
	return ltrim(rtrim(s));
}

inline std::string tolower(std::string s) {
	std::transform(
		s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);
	return s;
}

} // namespace robikzinputtest:text
