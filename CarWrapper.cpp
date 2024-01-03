#include "CarWrapper.h"
#include "IRSDK_Handler.h"

bool CarWrapper::is_valid(IRSDK_Handler* irsdk, int idx)
{
	return get_cartrackpos(irsdk, idx) >= 0;
}

bool CarWrapper::is_on_pit(IRSDK_Handler* irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).b_on_pitroad, false);
}

float CarWrapper::get_cartrackpos(IRSDK_Handler *irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).track_pct, -1.f);
}

char* CarWrapper::get_car_num(IRSDK_Handler* irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).num, nullptr);
}

int CarWrapper::get_car_pos(IRSDK_Handler* irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).pos, 0);
}

int CarWrapper::get_car_class_pos(IRSDK_Handler* irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).class_pos, 0);
}

int CarWrapper::get_class_color_r(IRSDK_Handler* irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).r, 255);
}
int CarWrapper::get_class_color_g(IRSDK_Handler* irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).g, 255);
}
int CarWrapper::get_class_color_b(IRSDK_Handler* irsdk, int idx)
{
	CHECK_VALID(irsdk->get_driver(idx).b, 255);
}

bool CarWrapper::is_in_player_class(IRSDK_Handler* irsdk, int idx, bool only_mine)
{
	if (only_mine && irsdk->is_connected()) {
		if (irsdk->get_player_id() > 0 && irsdk->get_player_id() < 63)
			return irsdk->get_driver(irsdk->get_player_id()).car_class == irsdk->get_driver(idx).car_class;
	}
	return true;
}