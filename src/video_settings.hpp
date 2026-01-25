#pragma once

#include "video_defs.hpp"

#include <SDL3/SDL.h>
#include <string>

namespace robikzinputtest {

class Settings;
struct VideoModeSettings;

std::string encode_display_id_memo(const DisplayIdMemo &display_id_memo);
DisplayIdMemo decode_display_id_memo(const std::string &encoded_display_id_memo);

void save_window_video_settings(Settings &settings, SDL_Window *window);
void load_window_video_settings(const Settings &settings, SDL_Window *window);

} // namespace robikzinputtest
