#pragma once

#include <SDL3/SDL.h>

namespace robikzinputtest {

class Settings;
struct VideoModeSettings;

void save_window_video_settings(Settings &settings, SDL_Window *window);
void load_window_video_settings(const Settings &settings, SDL_Window *window);

} // namespace robikzinputtest
