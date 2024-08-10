#ifndef KEVS_IRACING_COLLECTOR_H
#define KEVS_IRACING_COLLECTOR_H

#include "connection/data_collector.h"
#include "irsdk_client.h"

#define VALIDATE_INT(a,b) ((b >= 0 && b < MAX_TEAMS_IN_SESSION) ? a.getInt(b) : 0)

enum yaml_state {
	space,
	key,
	keysep,
	value,
	newline
};

class KevS_iRacing_Collector : public IKevS_DataCollector
{
private:
	static constexpr char *q_amnt_entries = "WeekendInfo:WeekendOptions:NumStarters:";
	static constexpr char *q_track_id = "WeekendInfo:WeekendOptions:NumStarters:";
	static constexpr char *q_pit_speed = "WeekendInfo:WeekendOptions:NumStarters:";
	static constexpr char *q_car_weight_bop = "DriverInfo:Drivers:CarIdx:{%d}CarClassWeightPenalty:";
	static constexpr char *q_class_colour = "DriverInfo:Drivers:CarIdx:{%d}CarClassColor:";
	static constexpr char *q_driver_id = "DriverInfo:Drivers:CarIdx:{%d}UserID:";
	static constexpr char *q_driver_name = "DriverInfo:Drivers:CarIdx:{%d}UserName:";
	static constexpr char *q_driver_name_short = "DriverInfo:Drivers:CarIdx:{%d}AbbrevName:";
	static constexpr char *q_team_name = "DriverInfo:Drivers:CarIdx:{%d}TeamName:";
	static constexpr char *q_car_number = "DriverInfo:Drivers:CarIdx:{%d}CarNumber:";
	
	char setup_name_template[256];

	irsdkCVar cvar_session_id;
	irsdkCVar cvar_car_position;
	irsdkCVar cvar_car_place;
	irsdkCVar cvar_car_class_place;
	//irsdkCVar cvar_car_position;
	//irsdkCVar cvar_car_position;

	void _fill_drivers_in_team() override;
	void _fill_teams() override;
	void _fill_session() override;

	static bool _parse_yaml(const char *data, const char* path, const char **val, int *len);
	static bool _parse_yaml_int(const char *yamlStr, const char *path, int *dest);
	static bool _parse_yaml_str(const char *yamlStr, const char *path, char *dest, int maxCount);
	bool _read_yaml_session_info(const char* yaml);
	bool _generate_setup_name_template();
	//void processYAMLSessionString(const char* yaml);

	void _reset();

public:
	KevS_iRacing_Collector();
	~KevS_iRacing_Collector();

	bool is_connected() override;
	void get_connection_status() override;
	
	void update_session();
	virtual float get_car_pos(int) override;

	virtual int get_overall_place(int) override;
	virtual char *get_number(int) override;
	virtual int get_class_place(int) override;
	virtual int get_class_colour_r(int) override;
	virtual int get_class_colour_g(int) override;
	virtual int get_class_colour_b(int) override;
	virtual int get_class(int) override;
	virtual int get_own_class() override;

	virtual bool is_on_pitroad(int) override;
};

#endif // KEVS_IRACING_COLLECTOR_H