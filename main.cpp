#include "ast.h"
#include "datalog-repair/repair.h"
#include "dockspace.h"
#include "editor.h"
#include "imgui-boilerplate/window.h"
#include "imgui.h"
#include <map>
#include <memory>

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
        editor.render(window::keyboard_input, window::text_input,
                      repair_program.get_possible_repairs(filename.c_str()));
        ui::ast::render(filename, repair_program.get_ast(filename.c_str()),
                        editor.get_buffer_position());
        // ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();
    }
    window::destroy();
    return 0;
}
