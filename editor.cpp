#include "editor.hpp"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <cstdlib>
#include <optional>
#include <iostream>

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
                        cursor.x = std::min(cursor.x, int(lines[cursor.y].size()));
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

void editor::render(std::queue<SDL_Keysym>& input, std::string& text_input, const std::vector<std::pair<int,int>>& repairs) {

    // returns intersection of two one-dimensional segments [a1,a2), [b1, b2)
    auto intersection = [](std::pair<int,int> a, std::pair<int,int> b)
        -> std::optional<std::pair<int,int>> {
        // order a and b so that a lies to the left of b
        if (a > b) std::swap(a, b);
        if (a.second <= b.first) return {};
        return std::pair(b.first, std::min(a.second, b.second));
        
    };

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
        int buf_pos = 0;
        for (auto& line : lines) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextDisabled("%02d", row);
            ImGui::TableNextColumn();
            // draw background
            for (auto selection : repairs) {
                auto is = intersection(selection, {buf_pos, buf_pos + line.size()});
                if (is) {
                    auto str_start = is->first - buf_pos;
                    auto str_end = is->second - buf_pos;
                    auto str = line.substr(str_start, str_end - str_start);
                    auto str_prefix = line.substr(0, str_start);

                    auto rect_upper_left = ImGui::GetCursorScreenPos() +
                        ImVec2(ImGui::CalcTextSize(str_prefix.c_str()).x, 0.0f);
                    auto rect_lower_right = rect_upper_left +
                                                    ImGui::CalcTextSize(str.c_str());
                    drawList->AddRectFilled(rect_upper_left,
                                            rect_lower_right,
                                            IM_COL32(255,200,200,255));
                    if (ImGui::IsMouseHoveringRect(rect_upper_left, rect_lower_right)) {
                        ImGui::BeginTooltip();
                        ImGui::Text("I am a tooltip");
                        ImGui::EndTooltip();
                    }
                }
            }
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
            buf_pos += line.size() + 1;
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
