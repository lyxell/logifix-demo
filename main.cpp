#include "imgui-boilerplate/window.hpp"
#include "imgui.h"
#include "dockspace.hpp"
#include "editor.hpp"

int main() {
    dockspace ds;
    editor ed;
    window::init();
    bool show_demo_window = true;
    while (!window::is_exiting()) {
        window::start_frame();

        ds.render();
        ed.render();

        // ImGui::ShowDemoWindow(&show_demo_window);

        window::end_frame();
    }
    window::destroy();
    return 0;
}
