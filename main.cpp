#include "imgui-boilerplate/window.hpp"
#include "imgui.h"
#include "dockspace.hpp"
#include "editor.hpp"
#include "build/sjp.hpp"
#include <memory>

using tuple = std::tuple<std::string,int,int>;

struct tuple_tree {
    tuple head;
    std::vector<std::shared_ptr<tuple_tree>> children;
};

bool tuple_contains(tuple a, tuple b) {
    return std::get<1>(a) <= std::get<1>(b) && std::get<2>(a) >= std::get<2>(b);
}

class parser_state {
    bool running = false;
    std::vector<std::string> data;
public:
    std::shared_ptr<tuple_tree> tuples;
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
        std::vector<tuple> ts = parser.get_tuples();
        std::sort(ts.begin(), ts.end(), [](tuple a, tuple b){
            return std::pair(std::get<1>(a), std::get<2>(b)) <
                   std::pair(std::get<1>(b), std::get<2>(a));
        });
        std::vector<std::shared_ptr<tuple_tree>> stack;
        tuples = nullptr;
        if (ts.size()) {
            tuples = std::make_shared<tuple_tree>(tuple_tree {ts[0]});
            stack.push_back(tuples);
        }
        for (size_t i = 1; i < ts.size(); i++) {
            while (!tuple_contains(stack.back()->head, ts[i])) stack.pop_back();
            auto ref = std::make_shared<tuple_tree>(tuple_tree {ts[i]});
            stack.back()->children.push_back(ref);
            stack.push_back(ref);
            
        }
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

void render_tuple_tree(std::shared_ptr<tuple_tree> t) {
    if (t) {
        auto [tuple, children] = *t;
        auto [sym, a, b] = tuple;
        if (ImGui::TreeNodeEx(t.get(), ImGuiTreeNodeFlags_DefaultOpen, "%s %d %d", sym.c_str(), a, b)) {
            for (auto child : children) {
                render_tuple_tree(child);
            }
            ImGui::TreePop();
        }
    }
}

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
        render_tuple_tree(ps.tuples);
        ImGui::End();
        ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();
    }
    window::destroy();
    return 0;
}
