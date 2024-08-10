#include "ui/Settings.h"
#include "imspinner.h"
#include "datatypes/data.h"
#include "defines.h"

void Settings::RenderUI(ImFont *font)
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
	ImGui::Begin("KevS", nullptr, window_flags);
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
	
	int main_segments = 64;
	int car_segments = 15;
	int car_radius = 15;
	int car_font_size = 12;
	int selected_numbering = 0;
	const char* number_items[] = { "Car-Number", "Class Position", "Overall Position" };
	float lost_time = 30.f;
	float lost_time_variance = 2.f;
	bool show_self_class_only = false;
	bool b_outcome_visible = true;
	bool b_outcome_transparent = true;
	bool b_outcome_on_top = true;

	ImGui::Begin("DoomWheel - Settings");
	ImGui::Text("Global");
	ImGui::DragInt("Main Segments", &main_segments, 2, 12, 256);
	ImGui::Checkbox("Only show my class", &show_self_class_only);
	ImGui::Combo("Number", &selected_numbering, number_items, IM_ARRAYSIZE(number_items));

	ImGui::Separator();
	ImGui::Text("Pit-Stop");
	ImGui::DragFloat("Sec lost in Pits", &lost_time, 1, 0, 300);
	ImGui::DragFloat("+/- Variance", &lost_time_variance, .1, 0, 10);
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("'Sec lost in Pits' used as mean-value.\n"
			"Variance is added before and after.\n"
			"i.e. Lost 20sec and Variance 3sec => 17-23sec\n"
			"If 0, than only one dot is rendered");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	ImGui::Checkbox("Visible", &b_outcome_visible);
	ImGui::SameLine();
	ImGui::Checkbox("Transparent", &b_outcome_transparent);
	ImGui::SameLine();
	ImGui::Checkbox("On Top", &b_outcome_on_top);

	ImGui::Separator();
	ImGui::Text("Car Circle");
	ImGui::DragInt("Car Segments", &car_segments, 2, 10, 45);
	ImGui::DragInt("Radius", &car_radius, 1, 5, 70);
	ImGui::DragInt("Font Size", &car_font_size, 1, 5, 45);

	ImGui::Separator();
	ImGui::Text("Session-Status:");
	ImGui::SameLine();
	if (KevS_Data::is_session_connected()) {
		ImGui::TextColored(ImVec4(.1f, .8f, .1f, 1.f), 
#if KEVS_USAGE_TYPE == IRACING_USAGE 
			"Connected to iRacing"
#elif KEVS_USAGE_TYPE == SECRET_USAGE
			"Connected to Secret-Service"
#else 
			"Connected to Session"
#endif
			);
	}
	else {
		ImGui::TextColored(ImVec4(.8f, .1f, .1f, 1.f), 
#if KEVS_USAGE_TYPE == IRACING_USAGE 
			"Disconnected from iRacing..."
#elif KEVS_USAGE_TYPE == SECRET_USAGE
			"Disconnected from Secret-Service..."
#else 
			"Disconnected from Session..."
#endif
		);
		ImGui::SameLine();
		ImSpinner::SpinnerBounceDots("Disc from Session" , 12.f, 4.f);
#ifdef IMSPINNER_DEMO
		ImSpinner::demoSpinners();
#endif
	}
#ifdef TESTING_IR
	static int chosen_driver = 0;
	ImGui::DragInt("Car Idx", &chosen_driver, 1, 0, 63);
	/*ImGui::Text("On Track: %.3f", irsdk->get_driver(chosen_driver).track_pct);
	ImGui::Text("In Pit: %d", irsdk->get_driver(chosen_driver).b_on_pitroad);
	ImGui::Text("Valid: %d", CarWrapper::is_valid(irsdk, chosen_driver));
	ImGui::Text("R:%d - G:%d - B:%d", irsdk->get_driver(chosen_driver).r,
		irsdk->get_driver(chosen_driver).g, irsdk->get_driver(chosen_driver).b);
	ImGui::Text("Color-Raw: %s", irsdk->get_driver(chosen_driver).raw_color);
	ImGui::Text("Own-Car-Class: %d", irsdk->get_own_class_id());*/
	// TODO: Remove after testing
	/*ImGui::Text("PlayerID: %d", irsdk->get_player_id());
	ImGui::Text("Avg N Laps: %.3f", irsdk->get_estimated_laptime());
	ImGui::Text("Track_Length: %.2f", irsdk->get_track_length());
	ImGui::Text("Track Sectors: %d", irsdk->get_amount_sectors());*/
#endif
	ImGui::End();
}