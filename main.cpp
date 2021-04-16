#include "imgui-boilerplate/window.hpp"
#include "imgui.h"
#include "dockspace.hpp"
#include "editor.hpp"
#include "build/sjp.hpp"

class parser_state {
    bool running = false;
    std::vector<std::string> data;
public:
    std::vector<std::tuple<std::string,int,int>> tuples;
    int num_asts;
    parser_state() {}
    void parse() {
        sjp::parser parser;
        std::string output;
        for (const auto& s : data) {
            output += s;
            output += "\n";
        }
        parser.add_string("Example.java", output.c_str());
        parser.parse();
        tuples = parser.get_tuples();
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
        ImGui::Begin("Data");
        ImGui::Text("Num ASTs:   %d", ps.num_asts);
        ImGui::Text("Cursor:     (%d,%d)", ed.cursor.x, ed.cursor.y);
        ImGui::Text("Buffer pos: %d", ed.get_buffer_position());
        ImGui::End();
        ImGui::Begin("Nodes");
        for (auto [t, a, b] : ps.tuples) {
            ImGui::Text("%s %d %d", t.c_str(), a, b);
        }
        ImGui::End();
        //ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();
    }
    window::destroy();
    return 0;
}
