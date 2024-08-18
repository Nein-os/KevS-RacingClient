#ifndef DOOMWHEEL_H
#define DOOMHWEEL_H

#include "imgui.h"
#include "../datatypes/doom_settings.h"

class IKevS_DataCollector;

class DoomWheel
{
public:
	static void RenderUI(ImFont *font, IKevS_DataCollector *data, DoomSettings *doom_wheel);

private:
    static void _draw_pit_outcome(float lost_time, float variance, ImVec2 win_center, 
        int radius, int car_radius, int car_segments, bool b_transparent, 
        IKevS_DataCollector *data);

    static ImVec2 _calc_center(ImVec2 pos, ImVec2 size);
    static ImVec2 _calc_right_bottom_corner(ImVec2 pos, ImVec2 size);
    static float _calc_radius(ImVec2 size);
    static ImVec2 _calc_car_position(ImVec2 pos, float radius, float track_pos);
    static float _calc_pct_of_pit_outcome(float, float);
};

#endif // DOOMWHEEL_H