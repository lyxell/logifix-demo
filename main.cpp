#include "imgui.h"
#include "imgui-boilerplate/window.hpp"
#include "dockspace.hpp"
#include "editor.hpp"
#include "datalog-repair/repair.hpp"
#include <memory>
#include <map>

std::map<std::tuple<std::string,int,int>,std::string> repairable_nodes;
std::map<std::tuple<std::string,int,int>,std::string> to_string;
std::vector<std::tuple<int,int,std::string>> rewrites;

void find_rewrites(std::shared_ptr<sjp::tree_node> node) {
    if (!node) return;
    auto it = repairable_nodes.find(std::tuple(node->get_name(),
                                         node->get_start_token(),
                                         node->get_end_token()));
    if (it != repairable_nodes.end()) {
        auto [k, v] = *it;
        rewrites.emplace_back(node->get_start_token(),
                              node->get_end_token(), v);
    }
    for (auto [symbol, children] : node->get_children()) {
        for (auto child : children) find_rewrites(child);
    }
}

void render_ast(std::string symbol,
        std::vector<std::shared_ptr<sjp::tree_node>> nodes, size_t pos) {
    auto is_hovered = [pos](std::shared_ptr<sjp::tree_node> node) {
        if (!node) return false;
        return node->get_start_token() <= pos
               && node->get_end_token() >= pos;
    };
    if (std::any_of(nodes.begin(), nodes.end(), is_hovered)) {
        ImGui::Text("%s:", symbol.c_str());
        for (auto t : nodes) {
            if (!t) continue;
            if (is_hovered(t)) {
                ImGui::Text("* %s", t->get_name().c_str());
                ImGui::Indent();
                for (auto [symbol, children] : t->get_children()) {
                    render_ast(symbol, children, pos);
                }
                ImGui::Unindent();
            } else {
                ImGui::TextDisabled("* %s", t->get_name().c_str());
            }
        }
    } else {
        ImGui::TextDisabled("%s", symbol.c_str());
    }
}

int main() {
    dockspace ds;
    editor ed;
    window::init();
    std::vector<std::string> prev_lines;
    std::shared_ptr<sjp::tree_node> ast;
    bool show_demo_window = true;
    while (!window::is_exiting()) {
        window::start_frame();

        ds.render();

        /*
        ImGui::Begin("Editor data");
        ImGui::Text("Cursor:     (%d,%d)", ed.cursor.x, ed.cursor.y);
        ImGui::Text("Buffer pos: %d", ed.get_buffer_position());
        ImGui::End();
        */

        /*
        ImGui::Begin("Repairs");
        for (auto [k, v] : repairable_nodes) {
            auto [str, a, b] = k;
            ImGui::Text("%s %d %d %s", str.c_str(), a, b, v.c_str());
        }
        ImGui::End();
        */

        /*
        ImGui::Begin("String representations");
        for (auto [k, v] : to_string) {
            auto [str, a, b] = k;
            ImGui::Text("%s %d %d", str.c_str(), a, b);
            ImGui::Text("%s", v.c_str());
        }
        ImGui::End();
        */

        ed.render(window::keyboard_input, window::text_input, rewrites);
        rewrites.clear();
        find_rewrites(ast);
        ImGui::Begin("AST");
        render_ast("root", {ast}, ed.get_buffer_position());
        ImGui::End();
        //ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();

        if (prev_lines != ed.lines) {
            repair rep;
            std::string output;
            for (const auto& s : ed.lines) {
                output += s;
                output += "\n";
            }
            rep.add_string("Example.java", output.c_str());
            rep.run();
            ast = rep.get_ast("Example.java");
            repairable_nodes = rep.get_repairable_nodes("Example.java");
            to_string = rep.get_string_representation("Example.java");
            prev_lines = ed.lines;
        }
    }
    window::destroy();
    return 0;
}
