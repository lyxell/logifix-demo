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
        std::vector<tuple> ts = parser.get_tuples("Example.java");
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

void render_tuple_tree(std::shared_ptr<tuple_tree> t, size_t pos) {
    if (t) {
        auto [tuple, children] = *t;
        auto [sym, a, b] = tuple;
        ImGui::Indent();
        if (a <= pos && pos <= b) {
            ImGui::Text("%s %d %d", sym.c_str(), a, b);
        } else {
            ImGui::TextDisabled("%s %d %d", sym.c_str(), a, b);
        }
        for (auto child : children) {
            render_tuple_tree(child, pos);
        }
        ImGui::Unindent();
    }
}

void set_style() {

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("FiraMono-Regular.ttf", 14.0f);
    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowMenuButtonPosition = -1;
    style->TabRounding = 0.0f;
    style->WindowBorderSize = 1.0f;
    ImVec4* c = style->Colors;
    c[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    c[ImGuiCol_TextDisabled]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    c[ImGuiCol_WindowBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    c[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    c[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    c[ImGuiCol_Border]                 = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    c[ImGuiCol_BorderShadow]           = ImVec4(0.75f, 0.75f, 0.75f, 0.00f);
    c[ImGuiCol_FrameBg]                = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    c[ImGuiCol_FrameBgHovered]         = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);
    c[ImGuiCol_FrameBgActive]          = ImVec4(0.79f, 0.79f, 0.79f, 0.67f);
    c[ImGuiCol_TitleBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    c[ImGuiCol_TitleBgActive]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    c[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    c[ImGuiCol_ScrollbarBg]            = ImVec4(0.78f, 0.78f, 0.78f, 0.53f);
    c[ImGuiCol_ScrollbarGrab]          = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    c[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    c[ImGuiCol_CheckMark]              = ImVec4(1.00f, 0.92f, 0.00f, 1.00f);
    c[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_Button]                 = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    c[ImGuiCol_ButtonHovered]          = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    c[ImGuiCol_ButtonActive]           = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
    c[ImGuiCol_Header]                 = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    c[ImGuiCol_HeaderHovered]          = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    c[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_Separator]              = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    c[ImGuiCol_SeparatorHovered]       = ImVec4(0.63f, 0.63f, 0.63f, 0.78f);
    c[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_ResizeGrip]             = ImVec4(0.78f, 0.78f, 0.78f, 0.20f);
    c[ImGuiCol_ResizeGripHovered]      = ImVec4(0.79f, 0.79f, 0.79f, 0.67f);
    c[ImGuiCol_ResizeGripActive]       = ImVec4(0.80f, 0.80f, 0.80f, 0.95f);
    c[ImGuiCol_Tab]                    = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
    c[ImGuiCol_TabHovered]             = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_TabActive]              = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_TabUnfocused]           = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
    c[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.81f, 0.81f, 0.81f, 1.00f);
    c[ImGuiCol_DockingPreview]         = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    c[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    c[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    c[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    c[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    c[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    c[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    c[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    c[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    c[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    c[ImGuiCol_TextSelectedBg]         = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
    c[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);
    c[ImGuiCol_NavHighlight]           = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
    c[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    c[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    c[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.91f, 0.00f, 1.00f);


}

int main() {
    parser_state ps;
    dockspace ds;
    editor ed;
    window::init();
    set_style();
    bool show_demo_window = true;
    while (!window::is_exiting()) {
        window::start_frame();
        ps.update(ed.lines);

        // Data
        ImGui::Begin("Data");
        ImGui::Text("Num ASTs:   %d", ps.num_asts);
        ImGui::Text("Cursor:     (%d,%d)", ed.cursor.x, ed.cursor.y);
        ImGui::Text("Buffer pos: %d", ed.get_buffer_position());
        ImGui::End();

        ds.render();
        ed.render(window::keyboard_input, window::text_input);
        ImGui::Begin("AST");
        ImGui::Unindent();
        render_tuple_tree(ps.tuples, ed.get_buffer_position());
        ImGui::Indent();
        ImGui::End();
        //ImGui::ShowDemoWindow(&show_demo_window);
        window::end_frame();
    }
    window::destroy();
    return 0;
}
