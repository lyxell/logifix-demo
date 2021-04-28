#pragma once

#include <SDL.h>
#include <queue>
#include <string>
#include <vector>

namespace ui {
struct cursor_t {
    int x;
    int y;
};

class editor {
  private:
    std::string filename;
    std::vector<std::string> lines;
    cursor_t cursor;
    void handle_keypress(std::queue<SDL_Keysym>&, std::string&);
    bool changed;

  public:
    editor(std::string filename);
    editor(std::string filename, std::vector<std::string> lines);
    auto render(std::queue<SDL_Keysym>&, std::string&,
                const std::vector<std::tuple<int, int, std::string>>&) -> void;
    auto get_buffer_position() -> size_t;
    auto get_source() -> std::string;
    auto has_changes() -> bool;
};
} // namespace ui
