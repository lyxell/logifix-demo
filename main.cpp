#include "dockspace.h"
#include "filepicker.h"
#include "ast.h"
#include "editor.h"
#include "imgui-boilerplate/window.h"
#include "imgui.h"
#include "state.h"
#include <dlfcn.h>
#include <iostream>
#include <thread>

static const std::string file = R"(import java.lang.Exception;
import java.io.*;
import java.util.function.IntFunction;
)";

const char* libpath = "./libprogram.so";

int main() {
    void* handle = nullptr;
    void* render_function = nullptr;
    state s = {.cursor = {0, 0}, .show_demo_window = false};
    std::istringstream f(file);
    std::string line;
    while (std::getline(f, line)) {
        s.lines.emplace_back(line);
    }
    std::string filename = "Example.java";
    window::init();
    while (!window::is_exiting()) {
        struct stat attr;
        if ((stat(libpath, &attr) == 0) && (s.inode != attr.st_ino)) {
            s.inode = attr.st_ino;
            std::cout << "changed" << std::endl;
            if (handle) {
                s.program = nullptr;
                dlclose(handle);
                handle = nullptr;
                render_function = nullptr;
            }
            handle = dlopen(libpath, RTLD_LAZY);
            if (handle == nullptr) {
                std::cerr << "could not connect to libprogram: " << dlerror()
                          << std::endl;
                s.inode = {};
            } else {
                std::cout << "new handle: " << handle << std::endl;
            }
            if (handle) {
                render_function = dlsym(handle, "run");
                if (!render_function) {
                    std::cerr << "could not load render function: " << dlerror()
                              << std::endl;
                    s.inode = {};
                } else {
                    s.dirty = true;
                }
            }
        }
        window::start_frame();
        s.keyboard_input = window::keyboard_input;
        s.text_input = window::text_input;
        //ui::dockspace::render();
        ui::filepicker::render(&s);

        size_t buffer_position = 0;
        for (int i = 0; i < s.cursor.second; i++) {
            buffer_position += s.lines[i].size() + 1; // 1 extra for newline
        }
        buffer_position +=
            std::min(s.cursor.first, int(s.lines[s.cursor.second].size()));

        if (s.program != nullptr) {
            const std::lock_guard<std::mutex> lock(s.mutex);
            ui::ast::render(&s, buffer_position);
        }

        ImGui::Begin("Location info");
        if (s.hovered_node && s.program) {
            const std::lock_guard<std::mutex> lock(s.mutex);
            auto [name, start, end] = s.program->get_node_properties(s.hovered_node);
            ImGui::Text("Hovered node: %s %d %d", name.c_str(), start, end);
            for (auto str : s.program->get_variables_in_scope(s.hovered_node)) {
                ImGui::Text("%s", str.c_str());
            }
        }
        ImGui::End();

        {
            const std::lock_guard<std::mutex> lock(s.mutex);
            ui::editor::render(&s);
        }

        /*{
            const std::lock_guard<std::mutex> lock(s.mutex);
            ImGui::Begin("Variables in scope");
            std::set<std::pair<std::string,std::string>> vars;
            for (auto [v, type, a, b] : s.variables_in_scope) {
                if (a <= buffer_position && b >= buffer_position) {
                    vars.emplace(v, type);
                }
            }
            if (ImGui::BeginTable("Declared variables", 2)) {
                ImGui::TableSetupColumn("Variable name");
                ImGui::TableSetupColumn("Type");
                ImGui::TableHeadersRow();
                for (auto [v, type] : vars) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", v.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", type.c_str());
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }*/

        if (s.show_demo_window) {
            ImGui::ShowDemoWindow();
        }
        if (render_function != nullptr && s.dirty) {
            s.dirty = false;
            auto fn = reinterpret_cast<void (*)(state*)>(render_function);
            std::thread t(fn, &s);
            t.detach();
        }
        window::text_input = {};
        window::end_frame();
    }
    window::destroy();
    return 0;
}
