#include "editor.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <tuple>

/*
size_t ui::editor::get_buffer_position() {
    size_t result = 0;
    for (int i = 0; i < cursor.y; i++) {
        result += lines[i].size() + 1; // 1 extra for newline
    }
    result += std::min(cursor.x, int(lines[cursor.y].size()));
    return result;
}

std::string ui::editor::get_source() {
    std::string output;
    for (const auto& s : lines) {
        output += s;
        output += "\n";
    }
    return output;
}
*/

void ui::editor::handle_keypress(state* s) {
    auto& lines = s->lines;
    auto& [x, y] = s->cursor;
    auto& input = s->keyboard_input;
    auto& text_input = s->text_input;
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
        if (!text_input.empty()) {
            x = std::min(x, int(lines[y].size()));
            lines[y].insert(x, text_input);
            x += text_input.size();
            text_input = {};
            s->dirty = true;
        }
        while (!input.empty()) {
            if (input.front().mod == KMOD_NONE) {
                switch (input.front().sym) {
                case SDLK_UP:
                    y = std::max(0, y - 1);
                    break;
                case SDLK_DOWN:
                    y = std::min(int(lines.size()) - 1, y + 1);
                    break;
                case SDLK_BACKSPACE:
                    s->dirty = true;
                    x = std::min(x, int(lines[y].size()));
                    if (x > 0) {
                        auto line = lines[y];
                        lines[y] = line.substr(0, x - 1) +
                                          line.substr(x);
                        x--;
                    } else if (y > 0) {
                        x = lines[y - 1].size();
                        lines[y - 1] += lines[y];
                        lines.erase(lines.begin() + y);
                        y--;
                    }
                    break;
                case SDLK_RIGHT:
                    x =
                        std::min(int(lines[y].size()), x + 1);
                    break;
                case SDLK_LEFT:
                    x = std::min(x, int(lines[y].size()));
                    x = std::max(0, x - 1);
                    break;
                case SDLK_RETURN:
                    s->dirty = true;
                    x = std::min(x, int(lines[y].size()));
                    lines.insert(lines.begin() + y + 1,
                                 lines[y].substr(x));
                    lines[y] = lines[y].substr(0, x);
                    y++;
                    x = 0;
                    break;
                }
            }
            input.pop();
        }
    }
}

void ui::editor::render(state* s) {

    auto& [x, y] = s->cursor;

    // returns intersection of two one-dimensional segments [a1,a2), [b1, b2)
    auto intersection =
        [](std::pair<int, int> a,
           std::pair<int, int> b) -> std::optional<std::pair<int, int>> {
        // order a and b so that a lies to the left of b
        if (a > b) {
            std::swap(a, b);
        }
        if (a.second <= b.first) {
            return {};
        }
        return std::pair(b.first, std::min(a.second, b.second));
    };

    ImGui::Begin("Editor");
    handle_keypress(s);
    auto* drawList = ImGui::GetWindowDrawList();
    const size_t NUM_COLUMNS = 2;
    static ImGuiTableFlags flags =
        ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Hideable;
    if (ImGui::BeginTable("Editor#table", NUM_COLUMNS, flags)) {
        ImGui::TableSetupColumn("Editor#A", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Editor#B", ImGuiTableColumnFlags_WidthStretch);
        int row = 1;
        int buf_pos = 0;
        for (auto& line : s->lines) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextDisabled("%02d", row);
            ImGui::TableNextColumn();
            // draw background
            for (auto selection : s->repairs) {
                auto is = intersection(
                    {std::get<0>(selection), std::get<1>(selection)},
                    {buf_pos, buf_pos + line.size()});
                if (is) {
                    auto str_start = is->first - buf_pos;
                    auto str_end = is->second - buf_pos;
                    auto str = line.substr(str_start, str_end - str_start);
                    auto str_prefix = line.substr(0, str_start);

                    auto rect_upper_left =
                        ImGui::GetCursorScreenPos() +
                        ImVec2(ImGui::CalcTextSize(str_prefix.c_str()).x, 0.0f);
                    auto rect_lower_right =
                        rect_upper_left + ImGui::CalcTextSize(str.c_str());
                    drawList->AddRectFilled(rect_upper_left, rect_lower_right,
                                            IM_COL32(255, 200, 200, 255));
                    if (ImGui::IsMouseHoveringRect(rect_upper_left,
                                                   rect_lower_right)) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Double-click to rewrite to %s",
                                    std::get<2>(selection).c_str());
                        ImGui::EndTooltip();
                    }
                    if (ImGui::IsMouseHoveringRect(rect_upper_left,
                                                   rect_lower_right) &&
                        ImGui::IsMouseDoubleClicked(0)) {
                        auto change = std::get<2>(selection);
                        std::string new_string = line.substr(0, str_start);
                        new_string += change;
                        new_string += line.substr(str_end);
                        line = std::move(new_string);
                        s->dirty = true;
                    }
                }
            }
            // draw cursor
            if (row - 1 == y) {
                ImVec2 pos = ImGui::GetCursorScreenPos();
                pos.x +=
                    ImGui::CalcTextSize(line.substr(0, x).c_str()).x;
                ImVec2 vstart(pos.x, pos.y);
                ImVec2 vend(pos.x + 1.0f, pos.y + ImGui::GetTextLineHeight());
                drawList->AddRectFilled(vstart, vend, IM_COL32(0, 0, 0, 255));
            }
            ImGui::Text("%s", line.c_str());
            row++;
            buf_pos += line.size() + 1;
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

