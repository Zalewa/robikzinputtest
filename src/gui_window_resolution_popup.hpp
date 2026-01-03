#pragma once

#include <chrono>
#include <memory>

namespace robikzinputtest {

struct VideoModeSettings;

namespace gui {

struct GuiContext;

/**
 * Confirmation pop-up for resolution change.
 */
class WindowResolutionPopup {
public:
	enum Result {
		CONTINUE,
		KEEP,
		REVERT,
	};

	WindowResolutionPopup();
	~WindowResolutionPopup();

	void set_deadline(
		std::chrono::steady_clock::time_point deadline
	);

	Result draw(const GuiContext &guictx);

private:
	struct D;
	std::unique_ptr<D> d;
};

} // namespace gui
} // namespace robikzinputtest
