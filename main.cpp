#include "imgui.h"
#include "imgui-boilerplate/window.hpp"
#include "dockspace.hpp"
#include "editor.hpp"
#include "datalog-repair/program.hpp"
#include <memory>
#include <map>

std::map<std::tuple<std::string,int,int>,
            std::tuple<std::string,int,int>> repairable_nodes;
std::vector<std::tuple<int,int,std::string>> rewrites;
std::vector<std::tuple<std::string,int,int,std::string>> pretty_print;

void render_ast(std::shared_ptr<sjp::tree_node> t, size_t pos) {
    if (t) {
        ImGui::Indent();
        for (auto& [key, value] : repairable_nodes) {
            auto& [k1, k2, k3] = key;
            auto& [v1, v2, v3] = value;
            if (k1 == t->get_name() &&
                k2 == t->get_start_token() &&
                k3 == t->get_end_token()) {
                for (auto [p1, p2, p3, p4] : pretty_print) {
                    if (p1 == v1 && p2 == v2 && p3 == v3) {
                        rewrites.emplace_back(k2, k3, p4);
                    }
                }
            }
        }
        if (t->get_start_token() <= pos && pos <= t->get_end_token()) {
            ImGui::Text("%s", t->get_name().c_str());
        } else {
            ImGui::TextDisabled("%s", t->get_name().c_str());
        }
        for (auto child : t->get_children()) {
            render_ast(child, pos);
        }
        ImGui::Unindent();
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
        ImGui::Begin("Pretty-print");
        for (auto& [t1,t2,t3,t4] : pretty_print) {
            ImGui::Text("%s %d %d %s", t1.c_str(), t2, t3, t4.c_str());
        }
        ImGui::End();
        */

        ed.render(window::keyboard_input, window::text_input, rewrites);
        rewrites.clear();
        ImGui::Begin("AST");
        ImGui::Unindent();
        render_ast(ast, ed.get_buffer_position());
        ImGui::Indent();
        ImGui::End();
        //ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();

        if (prev_lines != ed.lines) {
            repair::program program;
            std::string output;
            for (const auto& s : ed.lines) {
                output += s;
                output += "\n";
            }
            program.add_string("Example.java", output.c_str());
            program.run();
            ast = program.get_ast("Example.java");
            repairable_nodes = program.get_repairable_nodes("Example.java");
            prev_lines = ed.lines;
            pretty_print = program.get_pretty_print("Example.java");
        }
    }
    window::destroy();
    return 0;
}
