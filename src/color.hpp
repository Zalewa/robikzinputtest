#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace robikzinputtest {

inline float clamp_to_punit(float v) {
	return std::clamp(v, 0.0f, 1.0f);
}

template <typename T>
inline T punit_to_u8(float unit) {
	return static_cast<T>(clamp_to_punit(unit) * 255.f + 0.5f);
};

inline uint8_t hex_to_u8(char hi, char lo) {
	auto hexval = [](char c) -> uint8_t {
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		return 0; // unreachable due to prior check
	};
	return (hexval(hi) << 4) | hexval(lo);
};

inline float srgb_to_linear(float c) {
	if (c <= 0.04045f)
		return c / 12.92f;
	return std::pow((c + 0.055f) / 1.055f, 2.4f);
}

inline float hue_to_rgb(float p, float q, float t) {
	if (t < 0.0f)
		t += 1.0f;
	if (t > 1.0f)
		t -= 1.0f;

	if (t < 1.0f / 6.0f)
		return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f)
		return q;
	if (t < 2.0f / 3.0f)
		return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
	return p;
}

struct HSL;

/**
 * Represent 8-bit RGB color in range 0.0 - 1.0.
 */
struct Color {
	union {
		struct { float r, g, b, a; };
		float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

	const float &operator[](int idx) const { return value[idx]; }
	float &operator[](int idx) { return value[idx]; }

	float luminance() const {
		float lr = srgb_to_linear(this->r);
		float lg = srgb_to_linear(this->g);
		float lb = srgb_to_linear(this->b);
		return 0.2126f * lr + 0.7152f * lg + 0.0722f * lb;
	}

	bool is_light() const {
		// Threshold commonly used by Material Design and others.
		return this->luminance() > 0.5f;
	}

	bool is_dark() const {
		return !is_light();
	}

	/**
	 * Lighten or darken by delta.
	 *
	 * - positive delta: lighten
	 * - negative delta: darken
	 */
	Color adjust_brightness(float delta) const;

	std::string to_rgb_hex() const { return to_hex(false); }
	std::string to_rgba_hex() const { return to_hex(true); }
	std::string to_hex(bool alpha = false) const {
		std::ostringstream oss;
		oss << std::hex << std::setfill('0') << std::uppercase;
		oss << std::setw(2) << punit_to_u8<uint32_t>(r);
		oss << std::setw(2) << punit_to_u8<uint32_t>(g);
		oss << std::setw(2) << punit_to_u8<uint32_t>(b);
		if (alpha)
			oss << std::setw(2) << punit_to_u8<uint32_t>(a);
		return oss.str();
	}

	/**
	 * Interprets hexadecimal string into an RRGGBB or RRGGBBAA color.
	 *
	 * The string must not begin with any special character. It must
	 * only contain either 6 or 8 hexadecimal digits. The digits are
	 * case-insensitive. If 'AA' part is not specified, assume 1.0.
	 */
	static Color from_hex(const std::string &hexcolor, bool *ok = nullptr) {
		auto is_hex = [](char c) {
			return std::isxdigit(static_cast<unsigned char>(c));
		};

		if (!(hexcolor.size() == 6 || hexcolor.size() == 8)) {
			if (ok != nullptr)
				*ok = false;
			return {};
		}

		for (char c : hexcolor) {
			if (!is_hex(c)) {
				if (ok != nullptr)
					*ok = false;
				return {};
			}
		}

		Color c;
		c.r = hex_to_u8(hexcolor[0], hexcolor[1]) / 255.f;
		c.g = hex_to_u8(hexcolor[2], hexcolor[3]) / 255.f;
		c.b = hex_to_u8(hexcolor[4], hexcolor[5]) / 255.f;

		if (hexcolor.size() == 8)
			c.a = hex_to_u8(hexcolor[6], hexcolor[7]) / 255.f;
		else
			c.a = 1.0f;

		if (ok != nullptr)
			*ok = true;
		return c;
	}

	static Color from_hsl(const HSL &hsl);
};

template <typename T>
struct ColorU8 {
	union {
		struct { T r, g, b, a; };
		struct { T value[4]; };
	};

	const T &operator[](int idx) const { return value[idx]; }
	T &operator[](int idx) { return value[idx]; }

	static ColorU8 from(const Color &color) {
		ColorU8 cu8;
		cu8.r = punit_to_u8<T>(color.r);
		cu8.g = punit_to_u8<T>(color.g);
		cu8.b = punit_to_u8<T>(color.b);
		cu8.a = punit_to_u8<T>(color.a);
		return cu8;
	}
};

struct HSL {
	float h, s, l, a;

	static HSL from_color(const Color &color);
};

}
