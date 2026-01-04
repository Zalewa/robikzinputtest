#include "gui_logbox.hpp"

#include "gui_log.hpp"
#include "imgui_style.hpp"

#include <imgui.h>

namespace robikzinputtest::gui {

static void append_record(
	std::stringstream &ss,
	const char *record_begin,
	const char *record_end
) {
	// Make sure that all records end with a newline in the clipboard,
	// but also make sure that no extraneous newlines are added,
	// i.e. there should not be empty lines between records.
	if (record_end > record_begin && *(record_end - 1) == '\n')
		record_end--;
	std::string record(record_begin, record_end);
	ss << record << std::endl;
}

LogBox::LogBox() {
	m_auto_scroll = true;
}

void LogBox::draw(
	Log &log,
	const char *title,
	bool *p_open,
	float *opacity,
	std::function<void()> extra_options_callback
) {
	if (opacity != nullptr) {
		imgui::push_window_opacity(*opacity);
	}
	if (ImGui::Begin(title, p_open, ImGuiWindowFlags_NoMove)) {
		this->draw_body(log, opacity, extra_options_callback);
	}
	ImGui::End();
	if (opacity != nullptr) {
		imgui::pop_window_opacity();
	}
}

void LogBox::draw_body(Log &log, float *opacity, std::function<void()> extra_options_callback) {
	// Options menu
	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-scroll", &m_auto_scroll);
		if (opacity != nullptr)
			ImGui::SliderFloat("Opacity", opacity, 0.0f, 1.0f);
		extra_options_callback();
		ImGui::EndPopup();
	}

	// Main window
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();
	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	m_filter.Draw("Filter", -100.0f);

	ImGui::Separator();

	if (
		ImGui::BeginChild(
			"scrolling", ImVec2(0, 0),
			ImGuiChildFlags_None,
			ImGuiWindowFlags_HorizontalScrollbar
		)
	) {
		if (clear)
			log.clear();
		if (copy) {
			m_clipboard.clear();
		}

		const ImGuiTextBuffer &buffer = log.buffer();
		const std::vector<int64_t> &record_offsets = log.record_offsets();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = buffer.begin();
		const char* buf_end = buffer.end();
		if (m_filter.IsActive()) {
			for (size_t nrecord = 0; nrecord < record_offsets.size(); nrecord++) {
				const char* record_start = buf + record_offsets[nrecord];
				const char* record_end = (nrecord + 1 < record_offsets.size())
					? (buf + record_offsets[nrecord + 1] - 1)
					: buf_end;
				if (m_filter.PassFilter(record_start, record_end)) {
					if (copy && record_start != buf_end)
						append_record(m_clipboard, record_start, record_end);
					ImGui::TextUnformatted(record_start, record_end);
				}
			}
		} else {
			ImGuiListClipper clipper;
			clipper.Begin(record_offsets.size());
			while (clipper.Step()) {
				for (int nrecord = clipper.DisplayStart; nrecord < clipper.DisplayEnd; nrecord++) {
					const char* record_start = buf + record_offsets[nrecord];
					const char* record_end = (nrecord + 1 < record_offsets.size()) ? (buf + record_offsets[nrecord + 1] - 1) : buf_end;
					if (copy && record_start != buf_end)
						append_record(m_clipboard, record_start, record_end);
					ImGui::TextUnformatted(record_start, record_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (copy) {
			// The "ImGui::LogToClipboard()", taken from ImGUI's own demo, was
			// here before, but it would append extraneous newlines after each
			// record even if those were stripped from TextUnformatted. I was
			// unable to repair this, so "copy" runs via this now:
			ImGui::SetClipboardText(m_clipboard.str().c_str());
			m_clipboard.clear();
		}
		if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();
}

} // namespace robikzinputtest::gui
