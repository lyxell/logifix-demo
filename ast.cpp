#include "ast.hpp"
#include "imgui.h"

void ui::ast::render_node(std::shared_ptr<sjp::tree_node> node, size_t pos) {
    if (node == nullptr) return;
    auto is_hovered = [pos](std::shared_ptr<sjp::tree_node> n) {
        if (!n) return false;
        return n->get_start_token() <= pos
               && n->get_end_token() >= pos;
    };
    if (is_hovered(node)) {
        ImGui::Text("%s", node->get_name().c_str());
        ImGui::Indent();
        for (auto [symbol, child] : node->get_parent_of()) {
            auto Text = is_hovered(child) ? ImGui::Text : ImGui::TextDisabled;
            Text("%s:", symbol.c_str());
            ImGui::SameLine();
            if (child) render_node(child, pos);
            else Text("nil");
        }
        for (auto [symbol, children] : node->get_parent_of_list()) {
            bool hover =
                std::any_of(children.begin(), children.end(), is_hovered);
            auto Text = hover ? ImGui::Text : ImGui::TextDisabled;
            Text("%s:", symbol.c_str());
            ImGui::Indent();
            int counter = 0;
            for (auto child : children) {
                auto InnerText = is_hovered(child) ? ImGui::Text : ImGui::TextDisabled;
                ImGui::Unindent();
                InnerText("[%d]", counter);
                ImGui::Indent();
                ImGui::SameLine();
                render_node(child, pos);
                counter++;
            }
            if (!children.size()) {
                ImGui::SameLine();
                Text("nil");
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();
    } else {
        ImGui::TextDisabled("%s", node->get_name().c_str());
    }
}

void ui::ast::render(const std::string& filename,
            std::shared_ptr<sjp::tree_node> node,
            size_t position) {
    ImGui::Begin(("ui::ast " + filename).c_str());
    render_node(node, position);
    ImGui::End();
}
