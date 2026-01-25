#include "variant.hpp"

#include "video.hpp"
#include "video_settings.hpp"

#include <algorithm>

namespace robikzinputtest {

bool Variant::as_bool() const {
	std::string lowervalue(this->m_value.size(), '\0');
	std::transform(
		this->m_value.begin(), this->m_value.end(), lowervalue.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);
	return lowervalue == "true"
		|| lowervalue == "yes"
		|| lowervalue == "on"
		|| as_int64() != 0
		;
}

int32_t Variant::as_int32() const {
	return std::strtol(this->m_value.c_str(), nullptr, 10);
}

uint32_t Variant::as_uint32() const {
	return std::strtoul(this->m_value.c_str(), nullptr, 10);
}

int64_t Variant::as_int64() const {
	return std::strtoll(this->m_value.c_str(), nullptr, 10);
}

float Variant::as_float() const {
	return std::strtof(this->m_value.c_str(), nullptr);
}

double Variant::as_double() const {
	return std::strtod(this->m_value.c_str(), nullptr);
}

/*
  DisplayIdMemo
*/

Variant::Variant(const DisplayIdMemo &display_id_memo) :
	m_value(encode_display_id_memo(display_id_memo)) {}

Variant::operator DisplayIdMemo() const {
	return as_display_id_memo();
}

DisplayIdMemo Variant::as_display_id_memo() const {
	return decode_display_id_memo(this->m_value);
}

} // namespace robikzinputtest
