#include "editor.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imedit/imedit.h"
#include "imgui_internal.h"
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
                    x = std::min(int(lines[y].size()), x + 1);
                    break;
                case SDLK_LEFT:
                    x = std::min(x, int(lines[y].size()));
                    x = std::max(0, x - 1);
                    break;
                case SDLK_RETURN:
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

static void TextWithBackground(ImU32 color, const char* str) {
    static const auto HIGHLIGHT_PADDING = ImVec2(2.0f, 1.0f);
    static const auto HIGHLIGHT_ROUNDING = 2.0f;
    auto* drawList = ImGui::GetWindowDrawList();
    auto rect_upper_left = ImGui::GetCursorScreenPos() - HIGHLIGHT_PADDING;
    auto rect_lower_right =
        rect_upper_left + ImGui::CalcTextSize(str) + HIGHLIGHT_PADDING;

    drawList->AddRectFilled(rect_upper_left, rect_lower_right, color,
                            HIGHLIGHT_ROUNDING);
    ImGui::Text("%s", str);
}

/**
 * Returns intersection of two one-dimensional segments [a1,a2), [b1, b2)
 */
static std::optional<std::pair<int, int>>
find_intersection(std::pair<int, int> a, std::pair<int, int> b) {
    if (a > b) {
        std::swap(a, b);
    }
    if (a.second <= b.first) {
        return {};
    }
    return std::pair(b.first, std::min(a.second, b.second));
}

void ui::editor::render(state* s) {

    auto& [x, y] = s->cursor;

    ImGui::Begin("Editor");

    ImEdit::Begin("test");

    handle_keypress(s);
    for (auto& line : s->lines) {
        ImEdit::Line(line.c_str());
    }
    ImEdit::Cursor(s->cursor.first, s->cursor.second);

    int buffer_pos = 0;
    for (int row = 0; row < s->lines.size(); row++) {
        auto line = s->lines[row];
        for (auto [start, end, replacement] : s->repairs) {
            auto intersection = find_intersection(
                {start, end}, {buffer_pos, buffer_pos + line.size()});
            if (!intersection)
                continue;
            ImEdit::Highlight(row, start - buffer_pos, end - buffer_pos, []() {
                std::cout << "clicked" << std::endl;
                ImGui::OpenPopup("Stacked 1");
            });
        }
        buffer_pos += line.size() + 1;
    }

    ImEdit::End();

    if (ImGui::BeginPopupModal("Stacked 1")) {
        ImGui::Text("Hello from modal");
        TextWithBackground(IM_COL32(253, 184, 192, 255), "- hello");
        TextWithBackground(IM_COL32(172, 242, 189, 255), "+ hello");
        ImGui::EndPopup();
    }

    ImGui::End();
}
