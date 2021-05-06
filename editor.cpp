#include "editor.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imedit/imedit.h"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <tuple>

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
            s->set_scroll_to_cursor = true;
        }
        while (!input.empty()) {
            if (input.front().mod == KMOD_NONE) {
                switch (input.front().sym) {
                case SDLK_UP:
                    y = std::max(0, y - 1);
                    s->set_scroll_to_cursor = true;
                    break;
                case SDLK_DOWN:
                    y = std::min(int(lines.size()) - 1, y + 1);
                    s->set_scroll_to_cursor = true;
                    break;
                case SDLK_BACKSPACE:
                    s->dirty = true;
                    s->set_scroll_to_cursor = true;
                    x = std::min(x, int(lines[y].size()));
                    if (x > 0) {
                        auto line = lines[y];
                        lines[y] = line.substr(0, x - 1) + line.substr(x);
                        x--;
                    } else if (y > 0) {
                        x = lines[y - 1].size();
                        lines[y - 1] += lines[y];
                        lines.erase(lines.begin() + y);
                        y--;
                    }
                    break;
                case SDLK_RIGHT:
                    s->set_scroll_to_cursor = true;
                    x = std::min(int(lines[y].size()), x + 1);
                    break;
                case SDLK_LEFT:
                    s->set_scroll_to_cursor = true;
                    x = std::min(x, int(lines[y].size()));
                    x = std::max(0, x - 1);
                    break;
                case SDLK_RETURN:
                    s->set_scroll_to_cursor = true;
                    s->dirty = true;
                    x = std::min(x, int(lines[y].size()));
                    lines.insert(lines.begin() + y + 1, lines[y].substr(x));
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

/*

static const auto HIGHLIGHT_COLOR = IM_COL32(255, 243, 201, 255);
static const auto HIGHLIGHT_COLOR_HOVERED = IM_COL32(255, 224, 120, 255);
static const auto HIGHLIGHT_COLOR_CLICKED = IM_COL32(255, 212, 69, 255);
static const auto HIGHLIGHT_PADDING = ImVec2(2.0f, 1.0f);
static const auto HIGHLIGHT_ROUNDING = 2.0f;
static const auto SCROLL_OFFSET = 0.5f;

static void TextWithBackground(ImU32 color, const char* str) {
    auto* drawList = ImGui::GetWindowDrawList();
    auto rect_upper_left = ImGui::GetCursorScreenPos() -
            HIGHLIGHT_PADDING;
    auto rect_lower_right = rect_upper_left +
                            ImGui::CalcTextSize(str) +
            HIGHLIGHT_PADDING;

    drawList->AddRectFilled(rect_upper_left, rect_lower_right, color,
                            HIGHLIGHT_ROUNDING);
    ImGui::Text("%s", str);
}

// returns intersection of two one-dimensional segments [a1,a2), [b1, b2)
static std::optional<std::pair<int, int>> intersection(std::pair<int, int> a,
                                                       std::pair<int, int> b) {
    if (a > b) {
        std::swap(a, b);
    }
    if (a.second <= b.first) {
        return {};
    }
    return std::pair(b.first, std::min(a.second, b.second));
}

static void render_line(state* s, std::string& line, int& x, int& y,
                        int buf_pos, int row) {
    auto* drawList = ImGui::GetWindowDrawList();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextDisabled("%02d", row);
    ImGui::TableNextColumn();
    // draw highlights
    for (auto selection : s->repairs) {
        auto is = intersection({std::get<0>(selection), std::get<1>(selection)},
                               {buf_pos, buf_pos + line.size()});
        if (!is) {
            continue;
        }
        auto str_start = is->first - buf_pos;
        auto str_end = is->second - buf_pos;
        auto str = line.substr(str_start, str_end - str_start);
        auto str_prefix = line.substr(0, str_start);
        auto rect_upper_left =
            ImGui::GetCursorScreenPos() +
            ImVec2(ImGui::CalcTextSize(str_prefix.c_str()).x, 0.0f) -
            HIGHLIGHT_PADDING;
        auto rect_lower_right = rect_upper_left +
                                ImGui::CalcTextSize(str.c_str()) +
                                HIGHLIGHT_PADDING;
        auto rect_lower_left = ImVec2(rect_upper_left.x, rect_lower_right.y);
        auto hovered =
            ImGui::IsMouseHoveringRect(rect_upper_left, rect_lower_right);
        auto color = HIGHLIGHT_COLOR;
        auto clicked = hovered && ImGui::IsMouseDown(0);
        auto double_clicked = hovered && ImGui::IsMouseDoubleClicked(0);

        float value = 0.0f;

        ImGui::SetNextWindowPos(rect_lower_left + ImVec2(0.0f, 5.0f), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal(("repairpopup#" + std::to_string(row)).c_str()))
        {
            ImGui::Text("S1149");
            ImGui::Separator();
            ImGui::Text("Fix available", std::get<2>(selection).c_str());
            TextWithBackground(IM_COL32(253, 184, 192, 255),
                    ("- " + str).c_str());
            TextWithBackground(IM_COL32(172, 242, 189, 255),
                    ("+ " + std::get<2>(selection)).c_str());

            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        if (hovered) {
            color = HIGHLIGHT_COLOR_HOVERED;
        }
        if (clicked) {
            color = HIGHLIGHT_COLOR_CLICKED;
            ImGui::OpenPopup(("repairpopup#" + std::to_string(row)).c_str());
        }
        drawList->AddRectFilled(rect_upper_left, rect_lower_right, color,
                                HIGHLIGHT_ROUNDING);
        if (double_clicked) {
            auto change = std::get<2>(selection);
            std::string new_string = line.substr(0, str_start);
            y = row - 1;
            x = str_start + change.size();
            new_string += change;
            new_string += line.substr(str_end);
            line = std::move(new_string);
            s->dirty = true;
        }
    }
    // render cursor
    if (row - 1 == y) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        pos.x += ImGui::CalcTextSize(line.substr(0, x).c_str()).x;
        ImVec2 vstart(pos.x, pos.y);
        ImVec2 vend(pos.x + 1.0f, pos.y + ImGui::GetTextLineHeight());
        drawList->AddRectFilled(vstart, vend, IM_COL32(0, 0, 0, 255));
        if (s->set_scroll_to_cursor) {
            if (!ImGui::IsRectVisible(ImVec2(0.0f, vstart.y), vend)) {
                ImGui::SetScrollHereY(SCROLL_OFFSET);
            }
            s->set_scroll_to_cursor = false;
        }
    }
    ImGui::Text("%s", line.c_str());
}*/

void ui::editor::render(state* s) {


    auto& [x, y] = s->cursor;

    ImGui::Begin("Editor");

    ImEdit::Begin("test");

    handle_keypress(s);
    for (auto& line : s->lines) {
        ImEdit::Line(line.c_str());
    }
    ImEdit::Cursor(s->cursor.first, s->cursor.second);

    ImEdit::Highlight(1, 0, 5, [](){
        std::cout << "clicked" << std::endl;
        ImGui::OpenPopup("Stacked 1");
    });

    ImEdit::End();

    if (ImGui::BeginPopupModal("Stacked 1")) {
        ImGui::Text("Hello from modal");
        ImGui::EndPopup();
    }

    ImGui::End();

}
