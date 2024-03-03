#pragma once

typedef struct KevS_Entry_Data {
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
} Entry;