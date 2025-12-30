#pragma once

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <thread>

namespace robikzinputtest {

using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;
using Seconds = double;

template <typename T>
struct TimestampedValue {
	TimePoint at;
	T value;

	operator T&() const { return &value; }
	operator T&() { return &value; }
};

struct Countdown {
	Seconds remaining;

	Countdown() = default;
	Countdown(Seconds remaining) : remaining(remaining) {}

	/// Return true when countdown expires.
	bool countdown(Seconds amount) { remaining = std::max(0.0, remaining - amount); return is_expired(); }
	bool is_expired() const { return remaining <= 0.0; }
};

template <typename T>
struct ExpirableValue : public Countdown {
	T value;

	ExpirableValue() = default;
	ExpirableValue(Seconds remaining, T value)
		: Countdown(remaining), value(value) {}

	operator T&() const { return value; }
	operator T&() { return value; }
};

struct FrameTime {
	TimePoint prevtick;
	TimePoint currtick;
	Seconds delta_seconds;

	static FrameTime delta(
		const TimePoint &prevtick,
		const TimePoint &currtick
	) {
		auto elapsedNs = std::chrono::duration<uint64_t, std::nano>(currtick - prevtick).count();
		Seconds elapsedSec = elapsedNs / 1e9;
		return FrameTime { prevtick, currtick, elapsedSec };
	}
};

/**
 * A simple monotonic stop clock.
 *
 * Uses std::chrono::steady_clock which is guaranteed to be monotonic.
 */
class EngineClock {
public:
	EngineClock() :
		m_lasttick(std::chrono::steady_clock::now()),
		m_resolution(std::chrono::microseconds(1))
	{}

	/**
	 * Wait until the next tick (if necessary) and return the elapsed time
	 * since the last tick.
	 */
	const FrameTime tick() {
		// Sleep until the next tick target time.
		auto target = m_lasttick + m_resolution;
		auto sleep_time = target - std::chrono::steady_clock::now();
		if (sleep_time > std::chrono::nanoseconds::zero()) {
			SDL_DelayPrecise(std::chrono::duration_cast<std::chrono::nanoseconds>(sleep_time).count());
		}

		// Get the current time and compute the elapsed time since the last tick.
		auto now = std::chrono::steady_clock::now();
		FrameTime ts = FrameTime::delta(m_lasttick, now);
		m_lasttick = now;
		return ts;
	}

	/**
	 * Reset the clock to the current time.
	 */
	void reset() {
		m_lasttick = std::chrono::steady_clock::now();
	}

	/**
	 * Return the current instant of the clock.
	 */
	TimePoint lasttick() const {
		return m_lasttick;
	}

	void set_resolution(Duration resolution) {
		m_resolution = resolution;
	}

private:
	TimePoint m_lasttick;
	Duration m_resolution;
};

} // namespace robikzinputtest
