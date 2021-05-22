#pragma once

#include "logifix/logifix.h"
#include <SDL.h>
#include <mutex>
#include <queue>
#include <memory>
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

    ino_t inode;
    std::pair<int, int> cursor;
    bool show_demo_window;
    std::queue<SDL_Keysym> keyboard_input;
    int hovered_node;
    std::string text_input;
    bool dirty;
    std::mutex mutex;
    std::unique_ptr<logifix::program> program;
};
