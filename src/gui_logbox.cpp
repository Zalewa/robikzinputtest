#include "gui_logbox.hpp"

#include "gui_log.hpp"

#include <imgui.h>

namespace robikzinputtest::gui {

LogBox::LogBox() {
	m_auto_scroll = true;
}

void LogBox::draw(Log &log, const char *title, bool *p_open) {
	if (!ImGui::Begin(title, p_open)) {
		ImGui::End();
		return;
	}

	// Options menu
	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-scroll", &m_auto_scroll);
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
		if (copy)
			ImGui::LogToClipboard();

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
				if (m_filter.PassFilter(record_start, record_end))
					ImGui::TextUnformatted(record_start, record_end);
			}
		} else {
			ImGuiListClipper clipper;
			clipper.Begin(record_offsets.size());
			while (clipper.Step()) {
				for (int nrecord = clipper.DisplayStart; nrecord < clipper.DisplayEnd; nrecord++) {
					const char* record_start = buf + record_offsets[nrecord];
					const char* record_end = (nrecord + 1 < record_offsets.size()) ? (buf + record_offsets[nrecord + 1] - 1) : buf_end;
					ImGui::TextUnformatted(record_start, record_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();
	ImGui::End();
}

} // namespace robikzinputtest::gui
