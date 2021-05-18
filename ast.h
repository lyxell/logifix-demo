#pragma once
#include "logifix/logifix.h"
#include <string>

namespace ui::ast {
void render_node(logifix::program* program, int node, size_t pos);
void render(const std::string& filename, logifix::program* program, size_t pos);
} // namespace ui::ast
