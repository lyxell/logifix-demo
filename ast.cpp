#include "ast.h"
#include "imgui.h"

/*
void ui::ast::render_node(sjp::ast& ast, sjp::ast_node node, size_t pos) {
    if (node == 0) {
        return;
    }
    auto is_hovered = [&ast, pos](sjp::ast_node n) {
        if (n == 0) {
            return false;
        }
        return ast.starts_at[n] <= pos && ast.ends_at[n] >= pos;
    };
    if (is_hovered(node)) {
        ImGui::Text("%s", ast.name[node].c_str());
        ImGui::Indent();
        for (auto [symbol, child] : ast.parent_of[node]) {
            auto Text = is_hovered(child) ? ImGui::Text : ImGui::TextDisabled;
            Text("%s:", symbol.c_str());
            ImGui::SameLine();
            if (child) {
                render_node(ast, child, pos);
            } else {
                Text("nil");
            }
        }
        for (const auto& [symbol, children] : ast.parent_of_list[node]) {
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
                render_node(ast, child, pos);
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
        ImGui::TextDisabled("%s", ast.name[node].c_str());
    }
}

void ui::ast::render(const std::string& filename,
                     sjp::ast& ast,
                     size_t position) {
    ImGui::Begin((filename + " AST").c_str());
    ImGui::Text("%ld", position);
    render_node(ast, ast.root, position);
    ImGui::End();
}
*/
