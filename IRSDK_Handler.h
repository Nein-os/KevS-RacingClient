#pragma once

#include "irsdk/irsdk_client.h"

#define MAX_DRIVERS 64

typedef struct KevS_Driver_Data {
	bool b_initialized;
	bool b_on_pitroad;
	char num[4];
	int class_pos;
	int pos;
	float track_pct;
	int r, g, b;
	int car_class;
#ifdef TESTING_IR
	char raw_color[9];
#endif
	/*char name[64];
	char team_name[64];*/
} Driver;

class IRSDK_Handler
{
public:
	IRSDK_Handler();

private:
	bool running;
	Driver drivers[MAX_DRIVERS];

	irsdkCVar icv_car_track_pct;
	irsdkCVar icv_car_pos;
	irsdkCVar icv_car_class_pos;
	irsdkCVar icv_car_on_pitroad;
	irsdkCVar icv_own_caridx;
	irsdkCVar icv_avg_last_laps;

	bool init();
	void run();

	const char* generateLiveYAMLString();
	bool processYAMLLiveString();
	void processYAMLSessionString(const char* yaml);
	bool parceYAMLInt(const char* yaml, const char* path, int* dest);
	bool parceYAMLString(const char* yaml, const char* path, char* dest, int max);

	void update_driver_data();
	void reset();

public:
	void update();
	bool is_connected();
	void kill() { running = false; }
	int get_player_id(); // Returns 0 on invalid and -1 on not racing
	float get_estimated_laptime();

	Driver get_driver(int i) const { return drivers[i]; }
};

