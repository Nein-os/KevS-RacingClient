#include "connection/iracing_collector.h"
#include "datatypes/session.h"
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <pqxx/pqxx>

//char KevS_iRacing_Collector::a_amnt_entries[3];

KevS_iRacing_Collector::KevS_iRacing_Collector() : IKevS_DataCollector()
{
	cvar_session_id = irsdkCVar("SessionUniqueID");
	cvar_car_position = irsdkCVar("CarIdxLapDistPct");
	cvar_car_place = irsdkCVar("CarIdxPosition");
	cvar_car_class_place = irsdkCVar("CarIdxClassPosition");
	cvar_player_id = irsdkCVar("PlayerCarIdx");
	cvar_car_is_on_pitroad = irsdkCVar("CarIdxOnPitRoad");

	_reset();
}

void KevS_iRacing_Collector::_fill_session()
{
	char question[256];
	int id = 0;
	sprintf(question, q_track_id);
	if (_parse_yaml_int(irsdkClient::instance().getSessionStr(), question, &id)) {
		current_session->init(cvar_session_id.getInt(), id);
		_fill_teams();
	} else
		current_session->init(0, 0);
	//_read_yaml_session_info(irsdkClient::instance().getSessionStr());
}

void KevS_iRacing_Collector::_fill_teams()
{
	char question[256];
	char answer[64];
	char color[9];
	char number[3];
	int r, g, b;
	for (int i = 0; i < MAX_TEAMS_IN_SESSION; i++) {
		// HINT: In case of multi-threading malicious
		KevS_Team *team = current_session->get_team(i);
		assert(team != nullptr);
		if (!(team->is_initialized())) {
			sprintf(question, q_team_name, i);
			if (_parse_yaml_str(irsdkClient::instance().getSessionStr(), question, answer, sizeof(answer)-1)) {
				sprintf(question, q_class_colour, i);
				_parse_yaml_str(irsdkClient::instance().getSessionStr(), question, color, sizeof(color)-1);
				int value = 0;
				value = ((color[2] >= 'a') ? color[2] - 'a' + 10 : color[2] - '0') * 16;
				value += (color[3] >= 'a') ? color[3] - 'a' + 10 : color[3] - '0';
				r = value; value = 0;
				value = ((color[4] >= 'a') ? color[4] - 'a' + 10 : color[4] - '0') * 16;
				value += (color[5] >= 'a') ? color[5] - 'a' + 10 : color[5] - '0';
				g = value; value = 0;
				value = ((color[6] >= 'a') ? color[6] - 'a' + 10 : color[6] - '0') * 16;
				value += (color[7] >= 'a') ? color[7] - 'a' + 10 : color[7] - '0';
				b = value;

				sprintf(question, q_car_number, i);
				_parse_yaml_str(irsdkClient::instance().getSessionStr(), question, number, sizeof(number)-1);

				team->init(answer, r, g, b, number);
			}
		}
	}
	// no call to _fill_drivers_in_team
	// iRacing doesnt show all drivers in a team
	// Current drivers have to be monitored the whole time and saved to team accordingly
}

void KevS_iRacing_Collector::_fill_drivers_in_team()
{
	// Drivers need to be monitored all time
	char question[256];
	char answer[64];
	int id = 0;
	
	for (int i = 0; i < MAX_TEAMS_IN_SESSION; i++) {
		KevS_Team *team = current_session->get_team(i);
		assert(team != nullptr);
		if (team->is_initialized()) {
			sprintf(question, q_driver_name, i);
			if(_parse_yaml_str(irsdkClient::instance().getSessionStr(), question, answer, sizeof(answer)-1)) {
				sprintf(question, q_driver_id, i);
				_parse_yaml_int(irsdkClient::instance().getSessionStr(), question, &id);
				team->add_driver(id, answer);
			}
		}
	}
}

void KevS_iRacing_Collector::update_session()
{
	if (current_session->is_same_session(cvar_session_id.getInt())) {
		_fill_teams();
		_fill_drivers_in_team();
	} else {
		reset_session();
	}
}

bool KevS_iRacing_Collector::is_connected() { return irsdkClient::instance().isConnected(); }

void KevS_iRacing_Collector::get_connection_status() 
{
	// TODO
}


bool KevS_iRacing_Collector::_parse_yaml(const char *data, const char* path, const char **val, int *len)
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

bool KevS_iRacing_Collector::_parse_yaml_int(const char *yamlStr, const char *path, int *dest)
{
	if(dest) {
		(*dest) = 0;
		if(yamlStr && path) {
			int count;
			const char *strPtr;
			if(_parse_yaml(yamlStr, path, &strPtr, &count)) {
				(*dest) = atoi(strPtr);
				return true;
			}
		}
	}
	return false;
}

bool KevS_iRacing_Collector::_parse_yaml_str(const char *yamlStr, const char *path, char *dest, int maxCount)
{
	if(dest && maxCount > 0) {
		dest[0] = '\0';
		if(yamlStr && path) {
			int count;
			const char *strPtr;
			if(_parse_yaml(yamlStr, path, &strPtr, &count)) {
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

/*bool KevS_iRacing_Collector::_read_yaml_session_info(const char* yaml)
{
	char question[256];
	int track_id = 0;
	sprintf(question, q_track_id);
	if (_parse_yaml_int(yaml, question, &track_id)) {
		// TODO: get track
		try{
			pqxx::connection c("dbname=test user=postgres password=secret");
		} catch (const std::exception &e) {
			//std::cerr << e.what() << std::endl;
			return 1;
		}
	}
	return false;
}*/

// TODO: Rework
/*void KevS_iRacing_Collector::processYAMLSessionString(const char* yaml)
{
	char tstr[256];
	char car_class_str[10];
	char c_value[5];
	char length[6] = "\0\0\0\0\0";
	int value;
	for (int i = 0; i < MAX_TEAMS_IN_SESSION; i++) {
		if (current_session->get_team(i).is_initialized()) {
			char color[9];
			// skip the rest if carIdx not found
			sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}", i);
			if (parseYamlInt(yaml, tstr, &value)) {
				sprintf_s(tstr, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", i);
				parseYamlStr(yaml, tstr, c_value, 3); // 3 changed from 'sizeof(drivers[i].num)-1'
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
			}
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
		}
	}
}*/

float KevS_iRacing_Collector::get_car_pos(int idx)
{
	KevS_Team *team = current_session->get_team(idx);
	if (team && team->is_initialized())
		return cvar_car_position.getFloat(idx);
	return -1.f;
}

bool KevS_iRacing_Collector::_generate_setup_name_template()
{
	if (current_session->is_initialized()) {
		try{
			std::string db, user, password;
			std::ifstream file("settings");
			std::getline(file, db);
			std::getline(file, user);
			std::getline(file, password);
			pqxx::connection c("dbname=" + db + " user=" + user + " password=" + password);
			pqxx::work track_abrev{c};
			for (auto[abrev] : track_abrev.query<std::string>("SELECT track_code FROM tracks WHERE track_id=" 
					+ std::to_string(current_session->get_track_id()) + " LIMIT 1")) {
				// TODO: Use results and edit sql-string to match db
			}
			return true;
		} catch (const std::exception &e) {
			// TODO: Print error-message
			//std::cerr << e.what() << std::endl;
			return false;
		}
	}
	return false;
}

int KevS_iRacing_Collector::get_overall_place(int idx)
{
	return VALIDATE_INT(cvar_car_place, idx);
}
int KevS_iRacing_Collector::get_class_place(int idx)
{
	return VALIDATE_INT(cvar_car_class_place, idx);
}
char *KevS_iRacing_Collector::get_number(int idx)
{
	return current_session->get_team(idx)->get_car_number();
}
int KevS_iRacing_Collector::get_class_colour_r(int idx)
{
	return current_session->get_team(idx)->get_red();
}
int KevS_iRacing_Collector::get_class_colour_g(int idx)
{
	return current_session->get_team(idx)->get_green();
}
int KevS_iRacing_Collector::get_class_colour_b(int idx)
{
	return current_session->get_team(idx)->get_blue();
}
int KevS_iRacing_Collector::get_class(int)
{
	return 0; // TODO
}
int KevS_iRacing_Collector::get_own_class()
{
	return 0; // TODO
}
bool KevS_iRacing_Collector::is_on_pitroad(int idx)
{
	return VALIDATE_BOOL(cvar_car_is_on_pitroad, idx);
}

float KevS_iRacing_Collector::get_own_track_pos()
{
	return get_car_pos(cvar_player_id.getInt());
}

float KevS_iRacing_Collector::get_best_lap_time(int idx)
{
	return .0f; // TODO
}

float KevS_iRacing_Collector::get_self_best_lap_time()
{
	return get_best_lap_time(cvar_player_id.getInt());
}

void KevS_iRacing_Collector::_reset()
{
	reset_session();
	for (int i = 0; i < 256; i++)
		KevS_iRacing_Collector::setup_name_template[i] = '\0';
	// TODO: More reset needed?
}

KevS_iRacing_Collector::~KevS_iRacing_Collector() 
{
	// TODO
}