#pragma once

#include "state.h"
#include <SDL.h>
#include <queue>
#include <string>
#include <vector>

namespace ui::editor {
void render(state*);
void handle_keypress(state*);
//    size_t get_buffer_position();
//    std::string get_source();
//    bool has_changes();
} // namespace ui::editor
