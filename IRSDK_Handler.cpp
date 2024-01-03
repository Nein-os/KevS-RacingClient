#include "IRSDK_Handler.h"

#include "yaml_parser.h"
#include <thread>
#include <chrono>
#include <Windows.h>

IRSDK_Handler::IRSDK_Handler()
{
	running = true;
	reset();
	icv_car_pos = irsdkCVar("CarIdxPosition");
	icv_car_class_pos = irsdkCVar("CarIdxClassPosition");
	icv_car_on_pitroad = irsdkCVar("CarIdxOnPitRoad");
	icv_car_track_pct = irsdkCVar("CarIdxLapDistPct");
	icv_own_caridx = irsdkCVar("PlayerCarIdx");
	icv_avg_last_laps = irsdkCVar("LapLastNLapTime");
}


void IRSDK_Handler::update()
{
	if (init()) {
		using namespace std::chrono_literals;
		while (running) {
			run();
			update_driver_data();
			//std::this_thread::sleep_for(200ms);
		}
	}
	else {
		// TODO: Error Message
	}
}

void IRSDK_Handler::update_driver_data()
{
	for (int i = 0; i < MAX_DRIVERS; i++) {
		if (icv_car_pos.getInt(i) > 0) {
			drivers[i].b_on_pitroad = icv_car_on_pitroad.getBool(i);
			drivers[i].pos = icv_car_pos.getInt(i);
			drivers[i].class_pos = icv_car_class_pos.getInt(i);
			drivers[i].track_pct = icv_car_track_pct.getFloat(i);
			drivers[i].b_initialized = true;
		}
		else
			drivers[i].b_initialized = false;
	}
}

void IRSDK_Handler::reset()
{
	for (int i = 0; i < MAX_DRIVERS; i++) {
		drivers[i].b_initialized = false;
		for (int k = 0; k < 4; k++)
			drivers[i].num[k] = '\0';
		drivers[i].class_pos = -1;
		drivers[i].pos = -1;
	}
}

void IRSDK_Handler::processYAMLSessionString(const char* yaml)
{
	char tstr[256];
	char car_class_str[10];
	int value;
	for (int i = 0; i < MAX_DRIVERS; i++) {
		if (drivers[i].b_initialized) {
			char color[9];
			// skip the rest if carIdx not found
			sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}", i);
			if (parceYAMLInt(yaml, tstr, &(drivers[i].pos))) {
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", i);
				parceYAMLString(yaml, tstr, drivers[i].num, 3); // 3 changed from 'sizeof(drivers[i].num)-1'
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarClassColor:", i);
				parceYAMLString(yaml, tstr, color, 9);
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarClassID:", get_player_id());
				parceYAMLString(yaml, tstr, car_class_str, 10);
#ifdef TESTING_IR
				for (int j = 0; j < 8; j++)
					drivers[i].raw_color[j] = color[j];
				drivers[i].raw_color[8] = '\0';
#endif

				drivers[i].car_class = atoi(car_class_str);
				value = 0;
				value = ((color[2] >= 'a') ? color[2] - 'a' + 10 : color[2] - '0') * 16;
				value += (color[3] >= 'a') ? color[3] - 'a' + 10 : color[3] - '0';
				drivers[i].r = value; value = 0;
				value = ((color[4] >= 'a') ? color[4] - 'a' + 10 : color[4] - '0') * 16;
				value += (color[5] >= 'a') ? color[5] - 'a' + 10 : color[5] - '0';
				drivers[i].g = value; value = 0;
				value = ((color[6] >= 'a') ? color[6] - 'a' + 10 : color[6] - '0') * 16;
				value += (color[7] >= 'a') ? color[7] - 'a' + 10 : color[7] - '0';
				drivers[i].b = value;
			}
		}
	}
}

bool IRSDK_Handler::parceYAMLInt(const char* yaml, const char* path, int* dest)
{
	if (dest){
		(*dest) = 0;
		if (yaml && path) {
			int count;
			const char* strPtr;

			if (parseYaml(yaml, path, &strPtr, &count)) {
				(*dest) = atoi(strPtr);
				return true;
			}
		}
	}
	return false;
}

bool IRSDK_Handler::parceYAMLString(const char* yaml, const char* path, char* dest, int max)
{
	if (dest && max > 0) {
		dest[0] = '\0';
		if (yaml && path) {
			int count;
			const char* strPtr;
			if (parseYaml(yaml, path, &strPtr, &count)) {
				// strip leading quotes
				if (*strPtr == '"') {
					strPtr++;
					count--;
				}
				int l = (count > max) ? max : count;
				strncpy_s(dest, l, strPtr, l-1);
				dest[l] = '\0';
				if (l >= 1 && dest[l - 1] == '"')
					dest[l - 1] = '\0';

				return true;
			}
		}
	}
	return false;
}

bool IRSDK_Handler::is_connected()
{
	return irsdkClient::instance().isConnected();
}





bool IRSDK_Handler::init()
{
	// bump priority up so we get time from the sim
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	// ask for 1ms timer so sleeps are more precise
	timeBeginPeriod(1);
	//****Note, put your init logic here
	return true;
}

void IRSDK_Handler::run()
{
	// wait up to 16 ms for start of session or new data
	if (irsdkClient::instance().waitForData(16))
	{
		bool wasUpdated = false;

		// only process session string if it changed
		if (irsdkClient::instance().wasSessionStrUpdated())
		{
			processYAMLSessionString(irsdkClient::instance().getSessionStr());
			wasUpdated = true;
		}
	}
}

int IRSDK_Handler::get_player_id()
{
	return (irsdkClient::instance().isConnected()) ? icv_own_caridx.getInt() : -1;
}

float IRSDK_Handler::get_estimated_laptime()
{
	return (irsdkClient::instance().isConnected()) ? icv_avg_last_laps.getFloat() : -1;
}