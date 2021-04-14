#include "editor.hpp"
#include "imgui.h"
#include <cstdlib>

editor::editor() {
    lines = {
        "public class Main {",
        "    public static void main(String[] args) {",
        "       System.out.println();",
        "   }",
        "}",
        "MMXX"
    };
    cursor.x = 0;
    cursor.y = 0;
}

void editor::handle_keypress(std::queue<SDL_Keycode>& input) {
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
        while (input.size()) {
            switch (input.front()) {
                case SDLK_UP:
                    cursor.y = std::max(0, cursor.y - 1);
                    break;
                case SDLK_DOWN:
                    cursor.y = std::min(int(lines.size()) - 1, cursor.y + 1);
                    break;
                case SDLK_RIGHT:
                    cursor.x = std::min(int(lines[cursor.y].size()), cursor.x + 1);
                    break;
                case SDLK_LEFT:
                    cursor.x = std::max(0, cursor.x - 1);
                    break;
            }
            input.pop();
        }
    }
}

void editor::render(std::queue<SDL_Keycode>& input) {
    ImGui::Begin("Editor");
    handle_keypress(input);
	auto drawList = ImGui::GetWindowDrawList();
    const size_t NUM_COLUMNS = 2;
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_Hideable;
    if (ImGui::BeginTable("Editor#table", NUM_COLUMNS, flags)) {
        ImGui::TableSetupColumn("Editor#A", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Editor#B", ImGuiTableColumnFlags_WidthStretch);
        int row = 1;
        for (auto& line : lines) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%02d", row);
            ImGui::TableNextColumn();
            // draw cursor
            if (row - 1 == cursor.y) {
                ImVec2 pos = ImGui::GetCursorScreenPos();
                pos.x += ImGui::CalcTextSize(line.substr(0, cursor.x).c_str()).x;
                ImVec2 vstart(pos.x, pos.y);
                ImVec2 vend(pos.x + 1.0f, pos.y + ImGui::GetTextLineHeight());
                drawList->AddRectFilled(vstart, vend, IM_COL32(255,255,255,255));
            }
            ImGui::Text(line.c_str());
            row++;
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
