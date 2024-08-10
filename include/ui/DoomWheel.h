#ifndef DOOMWHEEL_H
#define DOOMHWEEL_H

#include "imgui.h"

class IKevS_DataCollector;

class DoomWheel
{
public:
	static void RenderUI(ImFont *font, IKevS_DataCollector *data);

private:
    static void draw_pit_outcome(float lost_time, float variance, ImVec2 win_center, 
        int radius, int car_radius, int car_segments, bool b_transparent, 
        IKevS_DataCollector *data);

    static ImVec2 calc_center(ImVec2 pos, ImVec2 size);
    static ImVec2 calc_right_bottom_corner(ImVec2 pos, ImVec2 size);
    static float calc_radius(ImVec2 size);
    static ImVec2 calc_car_position(ImVec2 pos, float radius, float track_pos);
};

#endif // DOOMWHEEL_H