#include "imgui.h"
#include "imgui-boilerplate/window.hpp"
#include "dockspace.hpp"
#include "editor.hpp"
#include "datalog-repair/program.hpp"
#include <memory>

std::vector<std::tuple<std::string,int,int>> repairable_nodes;
std::vector<std::pair<int,int>> repairs;

void render_ast(std::shared_ptr<sjp::tree_node> t, size_t pos) {
    if (t) {
        ImGui::Indent();
        for (auto& [a,b,c] : repairable_nodes) {
            if (a == t->get_name() && b == t->get_start_token() && c == t->get_end_token()) {
                repairs.emplace_back(
                        t->get_start_token(),
                        t->get_end_token());
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

        // Data
        ImGui::Begin("Data");
        ImGui::Text("Cursor:     (%d,%d)", ed.cursor.x, ed.cursor.y);
        ImGui::Text("Buffer pos: %d", ed.get_buffer_position());
        ImGui::End();

        ds.render();
        ed.render(window::keyboard_input, window::text_input, repairs);
        repairs.clear();
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
        }
    }
    window::destroy();
    return 0;
}
