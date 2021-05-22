#pragma once
#include "state.h"
#include <string>

namespace ui::ast {
void render_node(state* s, int node, size_t pos);
void render(state* s, size_t pos);
} // namespace ui::ast
