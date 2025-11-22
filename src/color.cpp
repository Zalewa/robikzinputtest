#include "color.hpp"
#include <algorithm>

namespace robikzinputtest {

/*
  Color
*/

Color Color::adjust_brightness(float delta) const {
	HSL hsl = HSL::from_color(*this);
	hsl.l = std::clamp(hsl.l + delta, 0.0f, 1.0f);
	return Color::from_hsl(hsl);
}

Color Color::from_hsl(const HSL &hsl) {
	float rr, gg, bb;

	if (hsl.s < 1e-6f) {
		rr = gg = bb = hsl.l;
	} else {
		float q = (hsl.l < 0.5f)
			? (hsl.l * (1.0f + hsl.s))
			: (hsl.l + hsl.s - hsl.l * hsl.s);
		float p = 2.0f * hsl.l - q;
		float hk = hsl.h / 360.0f;

		rr = hue_to_rgb(p, q, hk + 1.0f/3.0f);
		gg = hue_to_rgb(p, q, hk);
		bb = hue_to_rgb(p, q, hk - 1.0f/3.0f);
	}

	return Color{ rr, gg, bb, hsl.a };
}

/*
  HSL
*/

HSL HSL::from_color(const Color &color) {
	HSL hsl;
	hsl.a = color.a;

	float maxv = std::max(color.r, std::max(color.g, color.b));
	float minv = std::min(color.r, std::min(color.g, color.b));
	float d = maxv - minv;

	hsl.l = (maxv + minv) * 0.5f;

	if (d < 1e-6f) {
		hsl.h = hsl.s = 0.0f;
		return hsl;
	}

	hsl.s = (hsl.l > 0.5f)
		? (d / (2.0f - maxv - minv))
		: (d / (maxv + minv));

	float rc = (maxv - color.r) / d;
	float gc = (maxv - color.g) / d;
	float bc = (maxv - color.b) / d;

	if (color.r == maxv)
		hsl.h = (bc - gc);
	else if (color.g == maxv)
		hsl.h = 2.0f + (rc - bc);
	else
		hsl.h = 4.0f + (gc - rc);

	hsl.h = std::fmod((hsl.h * 60.0f) + 360.0f, 360.0f);
	return hsl;
}

} // namespace robikzinputtest
