#ifndef DOOM_SETTINGS_H
#define DOOM_SETTINGS_H

typedef struct KevS_Settings_for_Wheel_of_Doom
{
	int main_segments;
	int car_segments;
	int car_radius;
	int car_font_size;
	float lost_time;
	float lost_time_variance;
	int selected_numbering;
	bool show_self_class_only;
	bool b_outcome_transparent;
	bool b_outcome_on_top;
	bool b_outcome_visible;
	bool b_settings_collapsed;
	const char* number_items[3] = { "Car-Number", "Class Position", "Overall Position" };
} DoomSettings;

#endif // DOOM_SETTINGS_H