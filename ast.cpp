#include "ast.h"
#include "imgui.h"

void ui::ast::render_node(logifix::repair* program, int node, size_t pos) {
    if (node == 0) {
        return;
    }
    auto is_hovered = [program, pos](int n) {
        if (n == 0) {
            return false;
        }
        auto [name, starts_at, ends_at] = program->get_node_properties(n);
        return starts_at <= pos && ends_at >= pos;
    };
    auto [name, starts_at, ends_at] = program->get_node_properties(node);
    if (is_hovered(node)) {
        ImGui::Text("%s", name.c_str());
        ImGui::Indent();
        for (auto [symbol, child] : program->get_children(node)) {
            auto Text = is_hovered(child) ? ImGui::Text : ImGui::TextDisabled;
            Text("%s:", symbol.c_str());
            ImGui::SameLine();
            if (child) {
                render_node(program, child, pos);
            } else {
                Text("nil");
            }
        }
        for (const auto& [symbol, children] : program->get_child_lists(node)) {
            bool hover =
                std::any_of(children.begin(), children.end(), is_hovered);
            auto Text = hover ? ImGui::Text : ImGui::TextDisabled;
            Text("%s:", symbol.c_str());
            ImGui::Indent();
            int counter = 0;
            for (const auto& child : children) {
                auto InnerText =
                    is_hovered(child) ? ImGui::Text : ImGui::TextDisabled;
                ImGui::Unindent();
                InnerText("[%d]", counter);
                ImGui::Indent();
                ImGui::SameLine();
                render_node(program, child, pos);
                counter++;
            }
            if (children.empty()) {
                ImGui::SameLine();
                Text("nil");
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();
    } else {
        ImGui::TextDisabled("%s", name.c_str());
    }
}

void ui::ast::render(const std::string& filename,
                    logifix::repair* program,
                     size_t position) {
    ImGui::Begin((filename + " AST").c_str());
    ImGui::Text("%ld", position);
    render_node(program, program->get_root(), position);
    ImGui::End();
}

