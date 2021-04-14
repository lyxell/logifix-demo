#include "editor.hpp"
#include "imgui.h"

void editor::render() {
    ImGui::Begin("Editor");
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::InputTextMultiline("##source", text, 2048,
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
    ImGui::End();
}
