#include "editor.hpp"
#include "imgui.h"
#include <cstdlib>


editor::editor() {
    lines = {
        "import java.util.ArrayList;",
        "",
        "public class Main {",
        "    public static void main(String[] args) {",
        "        List<Integer> x = new ArrayList<>();",
        "        if (x.size() == 0) {",
        "            int x = 20;",
        "        }",
        "    }",
        "}",
    };
    cursor.x = 0;
    cursor.y = 0;
}

size_t editor::get_buffer_position() {
    size_t result = 0;
    for (int i = 0; i < cursor.y; i++)
        result += lines[i].size() + 1; // 1 extra for newline
    result += cursor.x;
    return result;
}

void editor::handle_keypress(std::queue<SDL_Keysym>& input, std::string& text_input) {
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
        if (text_input.size()) {
            lines[cursor.y].insert(cursor.x, text_input);
            cursor.x += text_input.size();
            text_input = {};
        }
        while (input.size()) {
            if (input.front().mod == KMOD_NONE) {
                switch (input.front().sym) {
                    case SDLK_UP:
                        cursor.y = std::max(0, cursor.y - 1);
                        break;
                    case SDLK_DOWN:
                        cursor.y = std::min(int(lines.size()) - 1, cursor.y + 1);
                        break;
                    case SDLK_BACKSPACE:
                        cursor.x = std::min(cursor.x, int(lines[cursor.y].size()));
                        if (cursor.x > 0) {
                            auto line = lines[cursor.y];
                            lines[cursor.y] = line.substr(0, cursor.x - 1) +
                                              line.substr(cursor.x);
                            cursor.x--;
                        } else if (cursor.y > 0) {
                           cursor.x = lines[cursor.y - 1].size();
                           lines[cursor.y - 1] += lines[cursor.y];
                           lines.erase(lines.begin() + cursor.y);
                           cursor.y--;
                        }
                        break;
                    case SDLK_RIGHT:
                        cursor.x = std::min(int(lines[cursor.y].size()), cursor.x + 1);
                        break;
                    case SDLK_LEFT:
                        cursor.x = std::min(cursor.x, int(lines[cursor.y].size()));
                        cursor.x = std::max(0, cursor.x - 1);
                        break;
                    case SDLK_RETURN:
                        lines.insert(lines.begin() + cursor.y + 1, lines[cursor.y].substr(cursor.x));
                        lines[cursor.y] = lines[cursor.y].substr(0, cursor.x);
                        cursor.y++;
                        cursor.x = 0;
                        break;
                }
            }
            input.pop();
        }
    }
}

void editor::render(std::queue<SDL_Keysym>& input, std::string& text_input) {
    ImGui::Begin("Editor");
    handle_keypress(input, text_input);
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
            ImGui::TextDisabled("%02d", row);
            ImGui::TableNextColumn();
            // draw cursor
            if (row - 1 == cursor.y) {
                ImVec2 pos = ImGui::GetCursorScreenPos();
                pos.x += ImGui::CalcTextSize(line.substr(0, cursor.x).c_str()).x;
                ImVec2 vstart(pos.x, pos.y);
                ImVec2 vend(pos.x + 1.0f, pos.y + ImGui::GetTextLineHeight());
                drawList->AddRectFilled(vstart, vend, IM_COL32(0,0,0,255));
            }
            ImGui::Text(line.c_str());
            row++;
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
