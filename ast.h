#pragma once
#include "squarelog/logifix.h"
#include <string>

namespace ui::ast {
void render_node(sjp::ast& ast, sjp::ast_node node, size_t pos);
void render(const std::string& filename, sjp::ast& ast, size_t pos);
} // namespace ui::ast
