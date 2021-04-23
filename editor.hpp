#pragma once

#include <vector>
#include <string>
#include <queue>
#include <SDL.h>

struct cursor_t {
    int x;
    int y;
};

class editor {
    void handle_keypress(std::queue<SDL_Keysym>&, std::string&);
public:
    std::vector<std::string> lines;
    cursor_t cursor;
    void render(std::queue<SDL_Keysym>&, std::string&,
            const std::vector<std::tuple<int,int,std::string>>&);
    size_t get_buffer_position();
    editor();
};
