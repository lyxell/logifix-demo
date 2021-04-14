#include "imgui-boilerplate/window.hpp"
#include "imgui.h"

void render_editor() {

    ImGui::Begin("Editor");

    // Note: we are using a fixed-sized buffer for simplicity here. See
    // ImGuiInputTextFlags_CallbackResize and the code in
    // misc/cpp/imgui_stdlib.h for how to setup InputText() for dynamically
    // resizing strings.

    static char text[1024 * 16] =
        "/*\n"
        " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
        " the hexadecimal encoding of one offending instruction,\n"
        " more formally, the invalid operand with locked CMPXCHG8B\n"
        " instruction bug, is a design flaw in the majority of\n"
        " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
        " processors (all in the P5 microarchitecture).\n"
        "*/\n\n"
        "label:\n"
        "\tlock cmpxchg8b eax\n";

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly",
            &flags, ImGuiInputTextFlags_ReadOnly);
    ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput",
            &flags, ImGuiInputTextFlags_AllowTabInput);
    ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine",
            &flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text),
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
    ImGui::End();
}

void render_dockspace() {

    bool p_open = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar |
                                    ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
    // render our background and handle the pass-thru hole, so we ask Begin()
    // to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    // Important: note that we proceed even if Begin() returns false (aka
    // window is collapsed). This is because we want to keep our DockSpace()
    // active. If a DockSpace() is inactive, all active windows docked into it
    // will lose their parent and become undocked. We cannot preserve the
    // docking relationship between an active window and an inactive docking,
    // otherwise any change of dockspace/settings would lead to windows being
    // stuck in limbo and never being visible.
    if (!opt_padding) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }

    ImGui::Begin("DockSpace Demo", &p_open, window_flags);

    if (!opt_padding) {
        ImGui::PopStyleVar();
    }

    if (opt_fullscreen) {
        ImGui::PopStyleVar(2);
    }

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }


    ImGui::End();
}

int main() {
    window::init();
    bool show_demo_window = true;
    bool show_another_window = false;
    while (!window::is_exiting()) {
        window::start_frame();

        render_dockspace();
        render_editor();

        ImGui::ShowDemoWindow(&show_demo_window);
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");
            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            if (ImGui::Button("Button")) {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
            ImGui::End();
        }
        if (show_another_window) {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        window::end_frame();
    }
    window::destroy();
    return 0;
}
