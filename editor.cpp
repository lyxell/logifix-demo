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

std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

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

void draw_child_window(state* s, const char* id, repair& r) {
    ImGui::Unindent();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(87, 87, 87, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, REPAIR_WINDOW_PADDING);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 4.0f));
    ImGui::Dummy(ImVec2(0.0f, 8.0f));
    ImGui::BeginChild(id, ImVec2(ImGui::GetWindowContentRegionWidth(), r.window_height), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
    float win_start = ImGui::GetCursorPosY();
    window::heading(r.message.c_str());

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
    window::text("Code smell");
    ImGui::PopStyleColor();
    ImGui::Text("");
    size_t buffer_pos = 0;
    size_t buffer_start = 0;
    std::string buffer;
    for (auto& line : s->lines) {
        auto intersection = find_intersection({r.start, r.end}, {buffer_pos, buffer_pos + line.size()});
        if (intersection) {
            if (buffer.empty()) buffer_start = buffer_pos;
            if (!buffer.empty()) buffer += "\n";
            buffer += line;
            ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.0f), "%s", line.c_str());
        }
        buffer_pos += line.size() + 1;
    }
    buffer = buffer.substr(0, r.start - buffer_start) + r.replacement + buffer.substr(r.end - buffer_start);
    for (auto s : split_string(buffer, "\n")) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", s.c_str());
    }
    ImGui::Text("");
    if (ImGui::Button("Close")) {
        r.open = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Apply")) {
        std::string result;
        for (auto& line : s->lines) {
            result += line += '\n';
        }
        result = result.substr(0, r.start) + r.replacement + result.substr(r.end);
        s->lines = {};
        for (auto str : split_string(result, "\n")) {
            s->lines.emplace_back(str);
        }
        s->dirty = true;
        r.open = false;
    }
    float win_end = ImGui::GetCursorPosY();
    r.window_height = win_end - win_start + 50.0f;
    ImGui::EndChild();
    ImGui::Dummy(ImVec2(0.0f, 8.0f));
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::Indent();
}

void ui::editor::render(state* s) {

    auto& [x, y] = s->cursor;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::Begin("Logifix Demo");

    ImEdit::Begin("test");
    ImGui::Indent();

    size_t row = 1;
    size_t buffer_pos = 0;
    for (auto& line : s->lines) {

        bool cursor_in_intersection = false;

        for (auto& [start, end, replacement, message, open, wh] : s->repairs) {
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
            ImEdit::Cursor(std::min(x, (int) line.size()));
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
                draw_child_window(s, ("child_id2" + std::to_string(row)).c_str(), rep);
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
