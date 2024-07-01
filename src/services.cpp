#include "services.h"
#include <iostream>
#include <string.h>
#include <chrono>
#include <thread>

#include "datatypes/driver.h"
#include "datatypes/team.h"
#include "irsdk_client.h"

void run_iracing_connection(bool *run)
{
	while (*run) {
		if (irsdkClient::instance().waitForData(16)) {
			if (irsdkClient::instance().wasSessionStrUpdated()) 
				processYAMLSessionString(irsdkClient::instance().getSessionStr());
			// TODO: update_driver_data();
		} else
			std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

bool parseYaml(const char *data, const char* path, const char **val, int *len)
{
	if(data && path && val && len) {
		*val = NULL;
		*len = 0;

		int depth = 0;
		yaml_state state = space;

		const char *keystr = NULL;
		int keylen = 0;
		const char *valuestr = NULL;
		int valuelen = 0;
		const char *pathptr = path;
		int pathdepth = 0;

		while(*data) {
			switch(*data) {
			case ' ':
				if(state == newline)
					state = space;
				if(state == space)
					depth++;
				else if(state == key)
					keylen++;
				else if(state == value)
					valuelen++;
				break;
			case '-':
				if(state == newline)
					state = space;
				if(state == space)
					depth++;
				else if(state == key)
					keylen++;
				else if(state == value)
					valuelen++;
				else if(state == keysep) {
					state = value;
					valuestr = data;
					valuelen = 1;
				}
				break;
			case ':':
				if(state == key) {
					state = keysep;
					keylen++;
				} else if(state == keysep) {
					state = value;
					valuestr = data;
				} else if(state == value)
					valuelen++;
				break;
			case '\n':
			case '\r':
				if(state != newline) {
					if(depth < pathdepth) 
						return false;
					else if(keylen && 0 == strncmp(keystr, pathptr, keylen)) {
						bool found = true;
						//do we need to test the value?
						if(*(pathptr+keylen) == '{') {
							//search for closing brace
							int pathvaluelen = keylen + 1; 
							while(*(pathptr+pathvaluelen) && *(pathptr+pathvaluelen) != '}')
								pathvaluelen++; 

							if(valuelen == pathvaluelen - (keylen+1) && 0 == strncmp(valuestr, (pathptr+keylen+1), valuelen))
								pathptr += valuelen + 2;
							else
								found = false;
						}

						if(found) {
							pathptr += keylen;
							pathdepth = depth;

							if(*pathptr == '\0') {
								*val = valuestr;
								*len = valuelen;
								return true;
							}
						}
					}

					depth = 0;
					keylen = 0;
					valuelen = 0;
				}
				state = newline;
				break;
			default:
				if(state == space || state == newline) {
					state = key;
					keystr = data;
					keylen = 0; //redundant?
				}
				else if(state == keysep) {
					state = value;
					valuestr = data;
					valuelen = 0; //redundant?
				}
				if(state == key)
					keylen++;
				if(state == value)
					valuelen++;
				break;
			}
			data++;
		}
	}
	return false;
}

bool parseYamlInt(const char *yamlStr, const char *path, int *dest)
{
	if(dest) {
		(*dest) = 0;
		if(yamlStr && path) {
			int count;
			const char *strPtr;
			if(parseYaml(yamlStr, path, &strPtr, &count)) {
				(*dest) = atoi(strPtr);
				return true;
			}
		}
	}
	return false;
}

bool parseYamlStr(const char *yamlStr, const char *path, char *dest, int maxCount)
{
	if(dest && maxCount > 0) {
		dest[0] = '\0';
		if(yamlStr && path) {
			int count;
			const char *strPtr;
			if(parseYaml(yamlStr, path, &strPtr, &count)) {
				// strip leading quotes
				if(*strPtr == '"') {
					strPtr++;
					count--;
				}
				int l = (count < maxCount) ? count : maxCount;
				strncpy(dest, strPtr, l);
				dest[l] = '\0';
				if(l >= 1 && dest[l-1] == '"')
					dest[l-1] = '\0';
				return true;
			}
		}
	}
	return false;
}


void processYAMLSessionString(const char* yaml)
{
	char tstr[256];
	char car_class_str[10];
	char length[6] = "\0\0\0\0\0";
	int value;
	for (int i = 0; i < MAX_TEAMS_IN_SESSION; i++) {
		if (current_session->get_team(i).is_initialized()) {
			char color[9];
			// skip the rest if carIdx not found
			sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}", i);
			/*if (parseYamlInt(yaml, tstr, &(drivers[i].pos))) {
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", i);
				parseYamlStr(yaml, tstr, drivers[i].num, 3); // 3 changed from 'sizeof(drivers[i].num)-1'
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarClassColor:", i);
				parseYamlStr(yaml, tstr, color, 9);
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarClassID:", get_player_id());
				parseYamlStr(yaml, tstr, car_class_str, 10);
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
			}*/
		}
	}
	sprintf_s(tstr, "WeekendInfo:TrackLength:");
	if (parseYamlStr(yaml, tstr, length, 5)) {
		// TODO
		/*float track_l = atof(length);
		if (need_reset(track_l)) {
			if (track_l > 0) {
				reset_track();
				current_track.length = track_l;
				init_new_track();
			}
		}*/
	}
}