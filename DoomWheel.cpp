#include "DoomWheel.h"

#include "imgui.h"
#include <math.h>

#define PI 3.14159265

void DoomWheel::RenderUI(ImFont *fonts[])
{
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
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
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
            if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
            if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
            ImGui::Separator();
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::Begin("DoomWheel - Settings");
    static int main_segments = 64;
    static int car_segments = 15;
    static int car_radius = 15;
    static float test_car_pos = 0;
    static bool b_car_filled = true;
    static int car_font_size = 12;
    static int amnt_car = 1;
    static float lost_time = 30.0f;
    static const char* font_items[] = { "Arial", "Courrier" };
    static int selected_font = 0;

    ImGui::Text("Global");
    ImGui::DragInt("Main Segments", &main_segments, 2, 12, 256);
    ImGui::DragInt("Amount Cars", &amnt_car, 1, 1, 25);
    ImGui::DragFloat("Sec lost in Pit", &lost_time, .02, 5, 120);
    ImGui::Combo("Font", &selected_font, font_items, IM_ARRAYSIZE(font_items));

    ImGui::Text("Car Circle");
    ImGui::DragInt("Car Segments", &car_segments, 1, 10, 45);
    ImGui::DragInt("Radius", &car_radius, 1, 5, 70);
    ImGui::Checkbox("Filled", &b_car_filled);
    ImGui::DragFloat("Test Car Move", &test_car_pos, .001, 0, 1);
    ImGui::DragInt("Font Size", &car_font_size, 1, 5, 25);
    ImGui::End();

    ImGui::Begin("DoomWheel - Viewport");
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();
    ImVec2 win_center = calc_center(win_pos, win_size);
    float radius = calc_radius(win_size);
    ImGui::GetForegroundDrawList()->AddCircle(win_center, radius,
        IM_COL32(150, 150, 150, 255), main_segments, 3);

    for (int i = 0; i < amnt_car; i++) {
        // TODO: get pos of CarIdx from iRacing
        // TODO: get class-color of CarIdx from iRacing
        // TODO: get car-num of CarIdx from iRacing
        // TODO: implement lost time signalling
        ImVec2 car_pos = calc_car_position(win_center, radius, test_car_pos + .07 * i);
        if (b_car_filled)
            ImGui::GetForegroundDrawList()->AddCircleFilled(car_pos, car_radius,
                IM_COL32(185, 35, 35, 255), car_segments);
        else
            ImGui::GetForegroundDrawList()->AddCircle(car_pos, car_radius,
                IM_COL32(185, 35, 35, 255), car_segments, 2);

        ImVec2 car_text_pos = ImVec2(car_pos.x - car_font_size,
            car_pos.y - car_font_size * .5);
        ImGui::GetForegroundDrawList()->AddText(fonts[selected_font], car_font_size,
            car_text_pos, IM_COL32(0, 0, 0, 255), "888");
    }
    ImGui::End();

    ImGui::ShowDemoWindow();

    ImGui::End();
}

ImVec2 DoomWheel::calc_center(ImVec2 pos, ImVec2 size)
{
    return ImVec2(pos.x + size.x * .5f, pos.y + size.y * .5f);
}

float DoomWheel::calc_radius(ImVec2 size)
{
    return ((size.x > size.y) ? size.y : size.x) * .4f;
}

ImVec2 DoomWheel::calc_car_position(ImVec2 pos, float radius, float track_pos)
{
    float degree = 2 * track_pos * PI;
    return ImVec2(sin(degree) * radius + pos.x,
        cos(degree) * -radius + pos.y);
}
