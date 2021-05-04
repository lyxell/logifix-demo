#pragma once

#include "ast.h"
#include <SDL.h>
#include <queue>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <vector>
#include <mutex>

struct state {

    std::vector<std::string> lines;
    std::weak_ptr<sjp::tree_node> ast;
    std::vector<std::tuple<int, int, std::string>> repairs;

    ino_t inode;
    std::pair<int, int> cursor;
    bool show_demo_window;
    std::queue<SDL_Keysym> keyboard_input;
    std::string text_input;
    bool dirty;
    bool set_scroll_to_cursor;
    std::mutex mutex;
};
