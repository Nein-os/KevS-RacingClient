#pragma once

#include "imgui.h"

class DoomWheel
{
public:
	static void RenderUI(ImFont *fonts[]);

private:
    static ImVec2 calc_center(ImVec2 pos, ImVec2 size);
    static float calc_radius(ImVec2 size);
    static ImVec2 calc_car_position(ImVec2 pos, float radius, float track_pos);
};

