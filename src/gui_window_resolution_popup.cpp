#include "gui_window_resolution_popup.hpp"

#include "app.hpp"
#include "gui_context.hpp"
#include "logger.hpp"

#include <imgui.h>
#include <SDL3/SDL.h>

#include <chrono>

namespace robikzinputtest::gui {

struct WindowResolutionPopup::D {
	std::chrono::steady_clock::time_point deadline = {};
	bool popup_open = false;
};

WindowResolutionPopup::WindowResolutionPopup()
	: d(std::make_unique<D>()) {
}

WindowResolutionPopup::~WindowResolutionPopup() = default;

void WindowResolutionPopup::set_deadline(
	std::chrono::steady_clock::time_point deadline
) {
	d->deadline = deadline;
}

WindowResolutionPopup::Result WindowResolutionPopup::draw(const GuiContext &guictx) {
	static const std::string window_title = "Resolution Confirmation";
	SDL_Window *const main_window = guictx.app.window();
	ImGuiIO &io = ImGui::GetIO();

	// The pop-up runs on a timeout where it will automatically return
	// to original resolution if no action is taken.
	const auto now = std::chrono::steady_clock::now();

	Result result = Result::CONTINUE;
	if (!ImGui::IsPopupOpen(window_title.c_str())) {
		if (!d->popup_open) {
			guictx.app.logger().info() << "Opening resolution confirmation popup" << std::endl;
			d->popup_open = true;
			ImGui::OpenPopup(window_title.c_str());
		} else {
			guictx.app.logger().info() << "Resolution confirmation popup was closed" << std::endl;
			d->popup_open = false;
			return Result::REVERT;
		}
	}
	ImGui::SetNextWindowFocus();
	// Keep the pop-up centered.
	ImGui::SetNextWindowPos(
		ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
		ImGuiCond_Always,
		ImVec2(0.5f, 0.5f)
	);
	if (
		ImGui::BeginPopupModal(
			window_title.c_str(),
			nullptr,
			ImGuiWindowFlags_AlwaysAutoResize
		)
	) {
		ImGui::Text("Do you want to keep the new resolution?");
		ImGui::Spacing();
		if (ImGui::Button("Keep")) {
			guictx.app.logger().info() << "Accepted new resolution" << std::endl;
			result = Result::KEEP;
			d->popup_open = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		// Focus on the revert button by default.
		if (ImGui::Button("Revert")) {
			result = Result::REVERT;
			d->popup_open = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
	if (result == Result::CONTINUE && now >= d->deadline) {
		guictx.app.logger().info() << "Resetting resolution to original settings" << std::endl;
		result = Result::REVERT;
	}
	return result;
}

} // namespace robikzinputtest::gui
