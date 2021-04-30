#pragma once

#include <SDL.h>
#include <queue>
#include <vector>
#include <utility>
#include <string>
#include <sys/stat.h>
#include "ast.h"

struct state {
    ino_t inode;
    std::vector<std::string> lines;
    std::vector<std::tuple<int,int,std::string>> repairs;
    std::pair<int,int> cursor;
    bool show_demo_window;
    std::queue<SDL_Keysym> keyboard_input;
    std::string text_input;
    bool dirty;
    std::weak_ptr<sjp::tree_node> ast;
};
