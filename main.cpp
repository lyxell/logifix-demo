#include "imgui-boilerplate/window.hpp"
#include "imgui.h"
#include "dockspace.hpp"
#include "editor.hpp"
#include "build/sjp.hpp"

class parser_state {
    bool running = false;
    std::vector<std::string> data;
public:
    size_t num_asts;
    parser_state() {}
    void parse() {
        sjp::parser parser;
        std::string output;
        for (const auto& s : data)
            output += s;
        parser.add_string("Example.java", output.c_str());
        parser.parse();
        num_asts = parser.num_asts();
    }
    void update(const std::vector<std::string>& u) {
        if (data != u) {
            data = u;
            std::cout << "updated data" << std::endl;
            parse();
        }
    }
};

int main() {
    parser_state ps;
    dockspace ds;
    editor ed;
    window::init();
    bool show_demo_window = true;
    while (!window::is_exiting()) {
        window::start_frame();
        ps.update(ed.lines);
        ds.render();
        ed.render(window::keyboard_input);
        ImGui::Begin("Number of ASTs");
        ImGui::Text(std::to_string(ps.num_asts).c_str());
        ImGui::End();
        ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();
    }
    window::destroy();
    return 0;
}
