#pragma once
#include "squarelog/squarelog.h"
#include <string>

namespace ui::ast {
void render_node(const std::shared_ptr<sjp::tree_node>& node, size_t pos);
void render(const std::string& filename,
            const std::shared_ptr<sjp::tree_node>& node, size_t position);
} // namespace ui::ast
