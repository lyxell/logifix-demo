#include "filepicker.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "imgui.h"

void ui::filepicker::render(state* s) {

    ImGui::Begin("Open file");
    if (ImGui::Button("Open File Dialog")) {
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFile", "Choose File", ".java", ".");
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFile")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
            std::cout << file_path_name << std::endl;
            std::ifstream infile(file_path_name);
            s->lines = {};
            std::string line;
            while (std::getline(infile, line)) {
                s->lines.emplace_back(line); 
            }
            s->dirty = true;
        }
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::End();

}
