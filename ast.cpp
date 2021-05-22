#include "ast.h"
#include "imgui.h"

void ui::ast::render_node(state* s, int node, size_t pos) {
    if (node == 0) {
        return;
    }
    auto is_hovered = [s, pos](int n) {
        if (n == 0) {
            return false;
        }
        auto [name, starts_at, ends_at] = s->program->get_node_properties(n);
        return starts_at <= pos && ends_at >= pos;
    };
    auto [name, starts_at, ends_at] = s->program->get_node_properties(node);
    if (is_hovered(node)) {
        s->hovered_node = node;
        ImGui::Text("%s", name.c_str());
        ImGui::Indent();
        for (auto [symbol, child] : s->program->get_children(node)) {
            auto Text = is_hovered(child) ? ImGui::Text : ImGui::TextDisabled;
            Text("%s:", symbol.c_str());
            ImGui::SameLine();
            if (child) {
                render_node(s, child, pos);
            } else {
                Text("nil");
            }
        }
        for (const auto& [symbol, children] : s->program->get_child_lists(node)) {
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
                render_node(s, child, pos);
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

void ui::ast::render(state *s, size_t position) {
    ImGui::Begin("AST");
    ImGui::Text("%ld", position);
    s->hovered_node = 0;
    render_node(s, s->program->get_root(), position);
    ImGui::End();
}

