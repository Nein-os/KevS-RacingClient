#include "Data_Handler.h"
#include "basics.h"

#ifdef USED_API_IRACING
	#include "irsdk/yaml_parser.h"
#endif
#include <thread>
#include <chrono>
#include <Windows.h>

IRSDK_Handler::IRSDK_Handler()
{
	running = true;
	current_track.sectors = nullptr;

	reset();
#ifdef USED_API_IRACING
	icv_car_pos = irsdkCVar("CarIdxPosition");
	icv_car_class_pos = irsdkCVar("CarIdxClassPosition");
	icv_car_on_pitroad = irsdkCVar("CarIdxOnPitRoad");
	icv_car_track_pct = irsdkCVar("CarIdxLapDistPct");
	icv_own_caridx = irsdkCVar("PlayerCarIdx");
	icv_avg_last_laps = irsdkCVar("LapLastNLapTime");
	icv_in_car = irsdkCVar("IsOnTrack");
#endif
}


void IRSDK_Handler::update()
{
	if (!init()) {
		using namespace std::chrono_literals;
		while (running) {
			run();
			// Till now, it doesnt seem to be needed
			//std::this_thread::sleep_for(200ms);
		}
	}
	else {
		// TODO: Error Message
		// Not possible to use a modal
		// TODO: Create whole logging-system
	}
}

void IRSDK_Handler::update_driver_data()
{
	for (int i = 0; i < MAX_ENTRIES; i++) {
		if (icv_car_pos.getInt(i) > 0) {
			entries[i].b_on_pitroad = icv_car_on_pitroad.getBool(i);
			entries[i].pos = icv_car_pos.getInt(i);
			entries[i].class_pos = icv_car_class_pos.getInt(i);
			entries[i].track_pct = icv_car_track_pct.getFloat(i);
			entries[i].b_initialized = true;
		}
		else
			entries[i].b_initialized = false;
	}
	if (current_track.sectors != nullptr)
		update_section_times();
}

void IRSDK_Handler::reset()
{
	for (int i = 0; i < MAX_ENTRIES; i++) {
		entries[i].b_initialized = false;
		for (int k = 0; k < 4; k++)
			entries[i].num[k] = '\0';
		entries[i].class_pos = -1;
		entries[i].pos = -1;
	}
	reset_track();
}

void IRSDK_Handler::processYAMLSessionString(const char* yaml)
{
	char tstr[256];
	char car_class_str[10];
	char length[6] = "\0\0\0\0\0";
	int value;
	for (int i = 0; i < MAX_ENTRIES; i++) {
		if (entries[i].b_initialized) {
			char color[9];
			// skip the rest if carIdx not found
			sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}", i);
			if (parceYAMLInt(yaml, tstr, &(entries[i].pos))) {
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", i);
				parceYAMLString(yaml, tstr, entries[i].num, 3); // 3 changed from 'sizeof(drivers[i].num)-1'
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarClassColor:", i);
				parceYAMLString(yaml, tstr, color, 9);
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarClassID:", get_player_id());
				parceYAMLString(yaml, tstr, car_class_str, 10);
#ifdef TESTING_IR
				for (int j = 0; j < 8; j++)
					entries[i].raw_color[j] = color[j];
				entries[i].raw_color[8] = '\0';
#endif

				entries[i].car_class = atoi(car_class_str);
				value = 0;
				value = ((color[2] >= 'a') ? color[2] - 'a' + 10 : color[2] - '0') * 16;
				value += (color[3] >= 'a') ? color[3] - 'a' + 10 : color[3] - '0';
				entries[i].r = value; value = 0;
				value = ((color[4] >= 'a') ? color[4] - 'a' + 10 : color[4] - '0') * 16;
				value += (color[5] >= 'a') ? color[5] - 'a' + 10 : color[5] - '0';
				entries[i].g = value; value = 0;
				value = ((color[6] >= 'a') ? color[6] - 'a' + 10 : color[6] - '0') * 16;
				value += (color[7] >= 'a') ? color[7] - 'a' + 10 : color[7] - '0';
				entries[i].b = value;
			}
		}
	}
	sprintf_s(tstr, "WeekendInfo:TrackLength:");
	if (parceYAMLString(yaml, tstr, length, 5)) {
		float track_l = atof(length);
		if (need_reset(track_l)) {
			if (track_l > 0) {
				reset_track();
				current_track.length = track_l;
				init_new_track();
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
		update_driver_data();
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

float IRSDK_Handler::get_track_length()
{
	return current_track.length;
}

void IRSDK_Handler::init_new_track()
{
	float length = get_track_length();
	int amnt_sectors = (int)(length / SECTOR_LENGTH);
	if (length == amnt_sectors) {}

	current_track.amnt_sectors = amnt_sectors;
	current_track.length = length;
	current_track.sectors = (float*)calloc(amnt_sectors, sizeof(float));
}

bool IRSDK_Handler::need_reset(float track_length)
{
	return track_length != current_track.length;
}

void IRSDK_Handler::reset_track()
{
	if (current_track.sectors != nullptr)
		delete current_track.sectors;
	current_track.length = .0f;
	current_track.amnt_sectors = 0;
	last_section_id = -1;
	last_time_stamp = std::chrono::steady_clock::now();
}

void IRSDK_Handler::update_section_times()
{
	int new_id = floor(entries[get_player_id()].track_pct
		* current_track.amnt_sectors);
	if (new_id > 0 && new_id != last_section_id) {
		last_section_id = new_id;
		std::chrono::steady_clock::time_point current =
			std::chrono::steady_clock::now();
		current_track.sectors[new_id] =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(current - last_time_stamp).count() / 1000.0f;
		last_time_stamp = current;
	}
}

float IRSDK_Handler::get_pct_of_pit_outcome(float lost_time)
{
	int calc_id = last_section_id;
	float calced_time = .0f;
	float calced_pct = .0f;
	if (icv_in_car.getBool() && current_track.sectors[2] > .0f) {
		while (calced_time < lost_time) {
			if (calc_id == 0)
				calc_id = get_amount_sectors();
			calced_time += current_track.sectors[--calc_id];
		}
		calced_pct = calc_id * 1.0f / get_amount_sectors();
	}
	return calced_pct;
}