#pragma once
#include "datalog-repair/repair.h"
#include <string>

namespace ui {
namespace ast {
void render_node(std::shared_ptr<sjp::tree_node> node, size_t pos);
void render(const std::string& filename, std::shared_ptr<sjp::tree_node> node,
            size_t position);
} // namespace ast
} // namespace ui
