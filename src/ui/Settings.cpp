#include "ui/Settings.h"
#include "imspinner.h"
#include "defines.h"

void Settings::RenderUI(ImFont *font, GeneralSettings *gen_settings, DoomSettings *doom_settings, bool con_status)
{
	ImGui::Begin("DoomWheel - Settings");
	ImGui::Text("Global");
	ImGui::DragInt("Main Segments", &doom_settings->main_segments, 2, 12, 256);
	ImGui::Checkbox("Only show my class", &doom_settings->show_self_class_only);
	ImGui::Combo("Number", &doom_settings->selected_numbering, 
		doom_settings->number_items, IM_ARRAYSIZE(doom_settings->number_items));

	ImGui::Separator();
	ImGui::Text("Pit-Stop");
	ImGui::DragFloat("Sec lost in Pits", &doom_settings->lost_time, 1, 0, 300);
	ImGui::DragFloat("+/- Variance", &doom_settings->lost_time_variance, .1, 0, 10);
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("'Sec lost in Pits' used as mean-value.\n"
			"Variance is added before and after.\n"
			"i.e. Lost 20sec and Variance 3sec => 17-23sec\n"
			"If 0, than only one dot is rendered");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	ImGui::Checkbox("Visible", &doom_settings->b_outcome_visible);
	ImGui::SameLine();
	ImGui::Checkbox("Transparent", &doom_settings->b_outcome_transparent);
	ImGui::SameLine();
	ImGui::Checkbox("On Top", &doom_settings->b_outcome_on_top);

	ImGui::Separator();
	ImGui::Text("Car Circle");
	ImGui::DragInt("Car Segments", &doom_settings->car_segments, 2, 10, 45);
	ImGui::DragInt("Radius", &doom_settings->car_radius, 1, 5, 70);
	ImGui::DragInt("Font Size", &doom_settings->car_font_size, 1, 5, 45);

	ImGui::Separator();
	ImGui::Text("Additional Windows");
	ImGui::Checkbox("DatabaseClient", &gen_settings->b_db_client_show);

	ImGui::Separator();
	ImGui::Text("Session-Status:");
	ImGui::SameLine();
	if (con_status) {
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