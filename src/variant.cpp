#include "variant.hpp"

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

int64_t Variant::as_int64() const {
	return std::strtoll(this->m_value.c_str(), nullptr, 10);
}

double Variant::as_double() const {
	return std::strtod(this->m_value.c_str(), nullptr);
}

} // namespace robikzinputtest
