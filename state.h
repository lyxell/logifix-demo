#pragma once

#include "ast.h"
#include <SDL.h>
#include <mutex>
#include <queue>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <vector>

struct repair {
    int start;
    int end;
    std::string replacement;
    std::string message;
    bool open;
    size_t window_height;
};

struct state {

    std::vector<std::string> lines;
    std::vector<repair> repairs;
    std::vector<std::tuple<std::string, std::string, int, int>> variables_in_scope;

    ino_t inode;
    std::pair<int, int> cursor;
    bool show_demo_window;
    std::queue<SDL_Keysym> keyboard_input;
    std::string text_input;
    bool dirty;
    std::mutex mutex;
    std::unique_ptr<logifix::program> program;
};
