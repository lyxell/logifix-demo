#include "imgui.h"
#include "ast.hpp"
#include "imgui-boilerplate/window.hpp"
#include "dockspace.hpp"
#include "editor.hpp"
#include "datalog-repair/repair.hpp"
#include <memory>
#include <map>

/*
std::map<std::tuple<std::string,int,int>,std::string> repairable_nodes;
std::map<std::tuple<std::string,int,int>,std::string> to_string;
std::map<std::tuple<std::string,int,int>,std::vector<std::string>> declared_vars;
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
    for (auto [symbol, child] : node->get_parent_of()) {
        find_rewrites(child);
    }
    for (auto [symbol, children] : node->get_parent_of_list()) {
        for (auto child : children) {
            find_rewrites(child);
        }
    }
}*/

int main() {
    std::string filename = "Example.java";
    repair repair_program;
    ui::dockspace dockspace;
    std::vector<std::string> lines = {
        "import java.util.ArrayList;",
        "",
        "public class Main {",
        "    public static void main(String[] args) {",
        "        ArrayList<Integer> x = new ArrayList<>();",
        "        if (x.size() == 0) {",
        "            System.out.println(\"empty\");",
        "        }",
        "    }",
        "}",
    };
    ui::editor editor(filename, lines);
    window::init();
    std::shared_ptr<sjp::tree_node> ast;
    bool show_demo_window = true;
    while (!window::is_exiting()) {

        if (editor.has_changes()) {
            repair_program = {};
            repair_program.add_string(filename.c_str(),
                                      editor.get_source().c_str());
            repair_program.run();
        }
        window::start_frame();

        dockspace.render();

        editor.render(window::keyboard_input, window::text_input, {});

        ui::ast::render(filename,
                        repair_program.get_ast(filename.c_str()),
                        editor.get_buffer_position());

        //ImGui::ShowDemoWindow(&show_demo_window);

        window::end_frame();

    }
    window::destroy();
    return 0;
}
