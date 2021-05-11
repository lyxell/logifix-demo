#include "editor.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imedit/imedit.h"
#include "imgui_internal.h"
#include "imgui-boilerplate/window.h"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <tuple>

static const auto REPAIR_WINDOW_PADDING = ImVec2(32.0f, 24.0f);
static const auto REPAIR_WINDOW_HEIGHT = 190.0f;

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

bool draw_child_window(std::vector<std::string>& lines, const char* id, repair& r) {
    bool close = false;
    ImGui::Unindent();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(87, 87, 87, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, REPAIR_WINDOW_PADDING);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::Dummy(ImVec2(0.0f, 8.0f));
    ImGui::BeginChild(id, ImVec2(ImGui::GetWindowContentRegionWidth(), REPAIR_WINDOW_HEIGHT), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
    window::heading(r.message.c_str());

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
    window::text("Code smell");
    ImGui::PopStyleColor();
    ImGui::Text("");
    size_t buffer_pos = 0;
    for (auto& line : lines) {
        auto intersection = find_intersection({r.start, r.end}, {buffer_pos, buffer_pos + line.size()});
        if (intersection) {
            ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.0f), "- %s", line.c_str());
        }
        buffer_pos += line.size() + 1;
    }
    //ImGui::TextColored(ImVec4(1.0f,1.0f,1.0f,1.0f), "+ %s", added);
    ImGui::Text("");
    close |= ImGui::Button("Close");
    ImGui::SameLine();
    close |= ImGui::Button("Apply");
    ImGui::EndChild();
    ImGui::Dummy(ImVec2(0.0f, 8.0f));
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::Indent();
    return close;
}

void ui::editor::render(state* s) {

    auto& [x, y] = s->cursor;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::Begin("Editor");

    ImEdit::Begin("test");
    ImGui::Indent();

    size_t row = 1;
    size_t buffer_pos = 0;
    for (auto& line : s->lines) {

        bool cursor_in_intersection = false;

        for (auto& [start, end, replacement, message, open] : s->repairs) {
            auto intersection = find_intersection(
                {start, end}, {buffer_pos, buffer_pos + line.size()});
            if (!intersection) continue;
            auto istart = intersection->first - buffer_pos;
            auto iend = intersection->second - buffer_pos;
            if (ImEdit::Underline(istart, iend)) {
                std::cout << "click " << replacement << std::endl;
                open = !open;
            }
        }

        if (row == y + 1) {
            ImEdit::Cursor(x);
        }

        ImEdit::Line(line.c_str());

        for (auto& rep : s->repairs) {
            auto intersection = find_intersection(
                {rep.start, rep.end}, {buffer_pos, buffer_pos + line.size()});
            // continue if there is no intersection
            if (!intersection) continue;
            // continue if we can place this box later
            if (rep.end > buffer_pos + line.size()) continue;
            auto istart = intersection->first - buffer_pos;
            auto iend = intersection->second - buffer_pos;

            if (rep.open) {
                bool close = draw_child_window(s->lines, ("child_id2" + std::to_string(row)).c_str(), rep);
                if (close) rep.open = false;
            }
        }

        row++;
        buffer_pos += line.size() + 1;
    }

    ImGui::Unindent();
    ImEdit::End();

    handle_keypress(s);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}
