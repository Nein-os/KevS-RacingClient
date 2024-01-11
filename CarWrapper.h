#pragma once

#include "Data_Handler.h"

#define CHECK_VALID(a,b) if (irsdk->get_entry(idx).b_initialized) return (a); else return (b)

namespace CarWrapper {
	bool is_valid(IRSDK_Handler*, int);
	bool is_on_pit(IRSDK_Handler*, int);
	float get_cartrackpos(IRSDK_Handler*, int);
	char* get_car_num(IRSDK_Handler*, int);
	int get_car_pos(IRSDK_Handler*, int);
	int get_car_class_pos(IRSDK_Handler*, int);
	int get_class_color_r(IRSDK_Handler*, int);
	int get_class_color_g(IRSDK_Handler*, int);
	int get_class_color_b(IRSDK_Handler*, int);
	bool is_in_player_class(IRSDK_Handler*, int, bool);
}