#pragma once

#include <chrono>
#include <thread>

namespace robikzinputtest {

using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;
using Seconds = double;

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
		std::this_thread::sleep_until(target);
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
