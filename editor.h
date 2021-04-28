#pragma once

#include <vector>
#include <string>
#include <queue>
#include <SDL.h>

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
        editor(const std::string& filename);
        editor(const std::string& filename, const std::vector<std::string>& lines);
        void render(std::queue<SDL_Keysym>&,
                    std::string&,
                    const std::vector<std::tuple<int,int,std::string>>&);
        size_t get_buffer_position();
        std::string get_source();
        bool has_changes();
    };
}