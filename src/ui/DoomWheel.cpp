#include "ui/DoomWheel.h"

//#define IMSPINNER_DEMO
#define TESTING_IR
//#define IMGUI_DEMO

#include "imgui.h"
#include <math.h>
//#include "CarWrapper.h"
#include "imspinner.h"
#include "irsdk_client.h"
#include "defines.h"
#include "connection/data_collector.h"
#include "connection/iracing_collector.h"

#define PI 3.14159265

struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	bool                ScrollToBottom;

	void    Clear() { Buf.clear(); }

	void    AddLog(const char* fmt, ...)// IM_PRINTFARGS(2)
	{
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_opened = NULL)
	{
		ImGui::Begin(title, p_opened);
		ImGui::TextUnformatted(Buf.begin());
		if (ScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;
		ImGui::End();
	}
};

void DoomWheel::RenderUI(ImFont *font, IKevS_DataCollector *data, DoomSettings *doom_wheel)
{
	ImGui::Begin("DoomWheel - Viewport");
	ImVec2 win_pos = ImGui::GetWindowPos();
	ImVec2 win_size = ImGui::GetWindowSize();
	ImVec2 win_center = _calc_center(win_pos, win_size);
	float radius = _calc_radius(win_size);
	ImGui::GetForegroundDrawList()->AddCircle(win_center, radius,
		IM_COL32(150, 150, 150, 255), doom_wheel->main_segments, 3);
	ImVec2 sf_line_tl = ImVec2(win_center.x - 2, win_center.y - radius - 10);
	ImVec2 sf_line_br = ImVec2(win_center.x + 2, win_center.y - radius + 13);
	ImGui::GetForegroundDrawList()->AddRectFilled(sf_line_tl, sf_line_br, IM_COL32(215, 0, 0, 255));
	
	if (doom_wheel->b_outcome_visible && !doom_wheel->b_outcome_on_top) 
		_draw_pit_outcome(doom_wheel->lost_time, doom_wheel->lost_time_variance, win_center, radius, doom_wheel->car_radius, doom_wheel->car_segments,
			doom_wheel->b_outcome_transparent, data);
			
	for (int i = 0; i < MAX_TEAMS_IN_SESSION; i++) {
		// TODO: Check if driver is in class and print if needed
		// TODO: get_car_pos leads to crash is_connected is fine
		if (data->get_car_pos(i) > 0 && (!doom_wheel->show_self_class_only || data->get_class(i) == data->get_own_class())) {
			ImVec2 car_pos = _calc_car_position(win_center, radius, data->get_car_pos(i));
			if (data->is_on_pitroad(i))
				ImGui::GetForegroundDrawList()->AddCircle(car_pos, (float)doom_wheel->car_radius,
					IM_COL32(data->get_class_colour_r(i), 
						data->get_class_colour_g(i),
						data->get_class_colour_b(i),
						255),
					doom_wheel->car_segments, 2);
			else
				ImGui::GetForegroundDrawList()->AddCircleFilled(car_pos, (float)doom_wheel->car_radius,
					IM_COL32(data->get_class_colour_r(i),
						data->get_class_colour_g(i),
						data->get_class_colour_b(i), 
						255), 
					doom_wheel->car_segments);

			ImVec2 car_text_pos = ImVec2(car_pos.x - doom_wheel->car_font_size * .5,
				car_pos.y - doom_wheel->car_font_size * .5);
			char* number = (char*)calloc(4, sizeof(char));
			switch (doom_wheel->selected_numbering) {
			case 1:
				sprintf(number, "%d", data->get_class_place(i));
				break;
			case 2:
				sprintf(number, "%d", data->get_overall_place(i));
				break;
			default:
				number = data->get_number(i);
			}
			ImGui::GetForegroundDrawList()->AddText(font, doom_wheel->car_font_size, car_text_pos, 
				data->is_on_pitroad(i) ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 0, 0, 255), 
				number);
		}
	}
	if (doom_wheel->b_outcome_visible && doom_wheel->b_outcome_on_top)
		_draw_pit_outcome(doom_wheel->lost_time, doom_wheel->lost_time_variance, win_center, radius, doom_wheel->car_radius, doom_wheel->car_segments,
			doom_wheel->b_outcome_transparent, data);
	/*ImGui::SetCursorScreenPos(_calc_right_bottom_corner(win_pos, win_size));
	if (ImGui::SmallButton((doom_wheel->b_settings_collapsed) ? "<" : ">")) 
		doom_wheel->b_settings_collapsed = (doom_wheel->b_settings_collapsed) ? false : true;*/
	
	ImGui::End();


#ifdef IMGUI_DEMO
	ImGui::ShowDemoWindow();
#endif
}

void DoomWheel::_draw_pit_outcome(float lost_time, float variance, ImVec2 win_center, 
	int radius, int car_radius, int car_segments, bool b_transparent, 
	IKevS_DataCollector *data)
{
	if (lost_time > 0.f && 
		#if KEVS_USAGE_TYPE == IRACING_USAGE
			data->get_own_track_pos()
		#else 
			0 // TODO
		#endif
		 > 0.f) {
		if (variance > 0.f) {
			float trailing_pct = _calc_pct_of_pit_outcome(lost_time + variance, data->get_self_best_lap_time());
			float leading_pct = _calc_pct_of_pit_outcome(lost_time - variance, data->get_self_best_lap_time());
			ImVec2 *outcome_dots = (ImVec2*)calloc(car_segments, sizeof(ImVec2));
			int amnt_dots_around = (car_segments - 4) / 2;
			float step_size_pct;

			// First cutted border
			outcome_dots[0] = _calc_car_position(win_center, radius - car_radius, trailing_pct);
			outcome_dots[1] = _calc_car_position(win_center, radius + car_radius, trailing_pct);

			// Calc dots till secound cutted border
			if (leading_pct < trailing_pct) {
				step_size_pct = 1 - trailing_pct;
				step_size_pct += leading_pct;
				step_size_pct /= amnt_dots_around;
			}
			else 
				step_size_pct = (leading_pct - trailing_pct) / amnt_dots_around;

			for (int i = 0; i < amnt_dots_around; i++) 
				outcome_dots[i + 2] = _calc_car_position(win_center, radius + car_radius, 
					trailing_pct + (i * step_size_pct));

			outcome_dots[2 + amnt_dots_around] = _calc_car_position(win_center, radius + car_radius, leading_pct);
			outcome_dots[3 + amnt_dots_around] = _calc_car_position(win_center, radius - car_radius, leading_pct);
			// Calc dots till first cutted border back
			for (int i = 0; i < amnt_dots_around; i++)
				outcome_dots[i + 4 + amnt_dots_around] = _calc_car_position(win_center, radius - car_radius, 
					leading_pct - (i * step_size_pct));

			outcome_dots[car_segments-1] = outcome_dots[0];
			for (int i = 0; i < car_segments; i++)
				printf("%d: x:%.2f y:%.2f\n", i, outcome_dots[i].x, outcome_dots[i].y);

			ImGui::GetForegroundDrawList()->AddPolyline(outcome_dots, car_segments, 
				IM_COL32(175, 95, 3, (b_transparent) ? 180 : 255), ImDrawFlags_None, 2);

			free(outcome_dots);
		}
		else {
			float lost_pct = _calc_pct_of_pit_outcome(lost_time, data->get_self_best_lap_time());
			ImVec2 outcome_pos = _calc_car_position(win_center, radius, lost_pct);
			ImGui::GetForegroundDrawList()->AddCircleFilled(outcome_pos, car_radius,
				IM_COL32(175, 95, 3, (b_transparent) ? 180 : 255), car_segments);
		}
	}
}

ImVec2 DoomWheel::_calc_center(ImVec2 pos, ImVec2 size)
{
	return ImVec2(pos.x + size.x * .5f, pos.y + size.y * .5f);
}

ImVec2 DoomWheel::_calc_right_bottom_corner(ImVec2 pos, ImVec2 size)
{
	return ImVec2(pos.x + size.x - 25, pos.y + size.y - 25);
}

float DoomWheel::_calc_radius(ImVec2 size)
{
	return ((size.x > size.y) ? size.y : size.x) * .4f;
}

ImVec2 DoomWheel::_calc_car_position(ImVec2 pos, float radius, float track_pos)
{
	float degree = 2 * track_pos * (float)PI;
	return ImVec2(sin(degree) * radius + pos.x,
		cos(degree) * -radius + pos.y);
}

float DoomWheel::_calc_pct_of_pit_outcome(float time, float ref)
{
	return (ref > 0) ? time / ref : 0.f;
}