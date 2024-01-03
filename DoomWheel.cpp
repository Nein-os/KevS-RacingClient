#include "DoomWheel.h"

//#define IMSPINNER_DEMO
//#define TESTING_IR
//#define IMGUI_DEMO

#include "imgui.h"
#include <math.h>
#include "IRSDK_Handler.h"
#include "CarWrapper.h"
#include "imspinner/imspinner.h"

#define PI 3.14159265

void DoomWheel::RenderUI(ImFont *fonts[], IRSDK_Handler *irsdk)
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
    static int car_font_size = 12;
    static float lost_time = 30.0f;
    static const char* font_items[] = { "Arial", "Courrier" };
    static int selected_font = 0;
    static int selected_numbering = 0;
    static const char* number_items[] = { "Car-Number", "Class Position", "Overall Position" };
    static bool show_self_class_only = false;
    static bool b_outcome_transparent = true;
    static bool b_outcome_first = true;
    static bool b_outcome_visible = true;

    ImGui::Text("Global");
    ImGui::DragInt("Main Segments", &main_segments, 2, 12, 256);
    ImGui::Checkbox("Only show my class", &show_self_class_only);
    ImGui::Combo("Number", &selected_numbering, number_items, IM_ARRAYSIZE(number_items));

    ImGui::Separator();
    ImGui::Text("Pit-Stop");
    ImGui::DragFloat("Sec lost in Pits", &lost_time, .02, 0, 300);
    ImGui::Checkbox("Visible", &b_outcome_visible);
    ImGui::SameLine();
    ImGui::Checkbox("Transparent", &b_outcome_transparent);
    ImGui::SameLine();
    ImGui::Checkbox("On Top", &b_outcome_first);

    ImGui::Separator();
    ImGui::Text("Car Circle");
    ImGui::DragInt("Car Segments", &car_segments, 1, 10, 45);
    ImGui::DragInt("Radius", &car_radius, 1, 5, 70);
    ImGui::DragInt("Font Size", &car_font_size, 1, 5, 25);
    ImGui::Combo("Font", &selected_font, font_items, IM_ARRAYSIZE(font_items));

    ImGui::Separator();
    ImGui::Text("Status");
    ImGui::SameLine();
    if (irsdk->is_connected()) {
        ImGui::TextColored(ImVec4(.1f, .8f, .1f, 1.f), "Connected");
    }
    else {
        ImGui::TextColored(ImVec4(.8f, .1f, .1f, 1.f), "Disconnected... ");
        ImGui::SameLine();
        ImSpinner::SpinnerBounceDots("Disc from iRacing", 12.f, 4.f);
#ifdef IMSPINNER_DEMO
        ImSpinner::demoSpinners();
#endif
    }
#ifdef TESTING_IR
    static int chosen_driver = 0;
    ImGui::DragInt("Car Idx", &chosen_driver, 1, 0, 63);
    ImGui::Text("On Track: %.3f", irsdk->get_driver(chosen_driver).track_pct);
    ImGui::Text("In Pit: %d", irsdk->get_driver(chosen_driver).b_on_pitroad);
    ImGui::Text("Valid: %d", CarWrapper::is_valid(irsdk, chosen_driver));
    ImGui::Text("R:%d - G:%d - B:%d", irsdk->get_driver(chosen_driver).r,
        irsdk->get_driver(chosen_driver).g, irsdk->get_driver(chosen_driver).b);
    ImGui::Text("Color-Raw: %s", irsdk->get_driver(chosen_driver).raw_color);
    ImGui::Text("Own-Car-Class: %d", irsdk->get_own_class_id());
#endif
    // TODO: Remove after testing
    ImGui::Text("Avg N Laps: %.3f", irsdk->get_estimated_laptime());
    ImGui::End();

    ImGui::Begin("DoomWheel - Viewport");
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();
    ImVec2 win_center = calc_center(win_pos, win_size);
    float radius = calc_radius(win_size);
    ImGui::GetForegroundDrawList()->AddCircle(win_center, radius,
        IM_COL32(150, 150, 150, 255), main_segments, 3);

    if (b_outcome_visible && b_outcome_first)
        draw_pit_outcome(lost_time, win_center, radius, car_radius, car_segments,
            b_outcome_transparent, irsdk);

    for (int i = 0; i < MAX_DRIVERS; i++) {
        if (CarWrapper::is_valid(irsdk, i) && CarWrapper::is_in_player_class(irsdk, i, show_self_class_only)) {
            ImVec2 car_pos = calc_car_position(win_center, radius, CarWrapper::get_cartrackpos(irsdk, i));
            if (CarWrapper::is_on_pit(irsdk, i))
                ImGui::GetForegroundDrawList()->AddCircle(car_pos, (float)car_radius,
                    IM_COL32(CarWrapper::get_class_color_r(irsdk, i), 
                        CarWrapper::get_class_color_g(irsdk, i),
                        CarWrapper::get_class_color_b(irsdk, i),
                        255),
                    car_segments, 2);
            else
                ImGui::GetForegroundDrawList()->AddCircleFilled(car_pos, (float)car_radius,
                    IM_COL32(CarWrapper::get_class_color_r(irsdk, i),
                        CarWrapper::get_class_color_g(irsdk, i),
                        CarWrapper::get_class_color_b(irsdk, i), 
                        255), 
                    car_segments);

            ImVec2 car_text_pos = ImVec2(car_pos.x - car_font_size * .5,
                car_pos.y - car_font_size * .5);
            char* number = (char*)calloc(4, sizeof(char));
            switch (selected_numbering) {
            case 1:
                sprintf(number, "%d", CarWrapper::get_car_class_pos(irsdk, i));
                break;
            case 2:
                sprintf(number, "%d", CarWrapper::get_car_pos(irsdk, i));
                break;
            default:
                number = CarWrapper::get_car_num(irsdk, i);
            }
            ImGui::GetForegroundDrawList()->AddText(fonts[selected_font], car_font_size, car_text_pos, 
                CarWrapper::is_on_pit(irsdk, i) ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 0, 0, 255), 
                number);
        }
    }
    if (b_outcome_visible && !b_outcome_first)
        draw_pit_outcome(lost_time, win_center, radius, car_radius, car_segments,
            b_outcome_transparent, irsdk);
    
    ImGui::End();


#ifdef IMGUI_DEMO
    ImGui::ShowDemoWindow();
#endif

    ImGui::End();
}

void DoomWheel::draw_pit_outcome(float lost_time, ImVec2 win_center, int radius,
    int car_radius, int car_segments, bool b_transparent, IRSDK_Handler* irsdk)
{
    if (lost_time > 0.f && irsdk->get_estimated_laptime() > 0
            && CarWrapper::get_cartrackpos(irsdk, irsdk->get_player_id()) > 0.f) {
        float lost_pct = lost_time / irsdk->get_estimated_laptime();
        lost_pct = CarWrapper::get_cartrackpos(irsdk, irsdk->get_player_id()) - lost_pct;
        ImVec2 car_pos = calc_car_position(win_center, radius, lost_pct);
        ImGui::GetForegroundDrawList()->AddCircle(car_pos, car_radius,
            IM_COL32(175, 95, 3, (b_transparent) ? 180 : 255), car_segments, 2);
    }
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
    float degree = 2 * track_pos * (float)PI;
    return ImVec2(sin(degree) * radius + pos.x,
        cos(degree) * -radius + pos.y);
}
