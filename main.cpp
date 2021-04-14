#include "imgui-boilerplate/window.hpp"
#include "imgui.h"
#include "dockspace.hpp"
#include "editor.hpp"
#include "build/sjp.hpp"

class parser_state {
    bool running = false;
    std::string data;
public:
    size_t num_asts;
    parser_state() {}
    void parse(std::string str) {
        sjp::parser parser;
        parser.add_string("Example.java", str.c_str());
        parser.parse();
        num_asts = parser.num_asts();
    }
    void update(const std::string& u) {
        if (data != u) {
            data = u;
            std::cout << "updated data" << std::endl;
            parse(data);
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
        ps.update(std::string(std::begin(ed.text), std::end(ed.text)));
        ds.render();
        ed.render();
        ImGui::Begin("Number of ASTs");
        ImGui::Text(std::to_string(ps.num_asts).c_str());
        ImGui::End();
        // ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();
    }
    window::destroy();
    return 0;
}
