#pragma once

#include "color.hpp"

#include <string>

namespace robikzinputtest {

class Variant {
public:
	Variant() : m_valid(false) {}
	Variant(std::string s) : m_value(s) {}
	Variant(bool b) : m_value(b ? "true" : "false") {}
	Variant(Color color) : m_value(color.to_rgba_hex()) {}
	Variant(int32_t i) : m_value(std::to_string(i)) {}
	Variant(uint32_t i) : m_value(std::to_string(i)) {}
	Variant(int64_t i) : m_value(std::to_string(i)) {}
	Variant(double d) : m_value(std::to_string(d)) {}

	operator std::string() const { return as_string(); }
	operator Color() const { return as_color(); }
	operator bool() const { return as_bool(); }
	operator int32_t() const { return as_int32(); }
	operator uint32_t() const { return as_uint32(); }
	operator int64_t() const { return as_int64(); }
	operator float() const { return as_float(); }
	operator double() const { return as_double(); }

	const std::string &as_string() const { return m_value; }
	bool as_bool() const;
	Color as_color() const { return Color::from_hex(m_value); }
	int32_t as_int32() const;
	uint32_t as_uint32() const;
	int64_t as_int64() const;
	float as_float() const;
	double as_double() const;

	bool valid() const { return m_valid; }

	bool operator==(const Variant &other) const {
		if (!m_valid && !other.m_valid)
			return true;
		if (m_valid != other.m_valid)
			return false;
		return m_value == other.m_value;
	}

	bool operator!=(const Variant &other) const {
		return !(*this == other);
	}

private:
	bool m_valid = true;
	std::string m_value;
};

} // namespace robikzinputtest
