#ifndef KEVS_IRACING_COLLECTOR_H
#define KEVS_IRACING_COLLECTOR_H

#include "connection/data_collector.h"
#include "irsdk_client.h"

class KevS_iRacing_Collector : public IKevS_DataCollector
{
private:
	static constexpr char *q_amnt_entries = "WeekendInfo:WeekendOptions:NumStarters:";
	static char a_amnt_entries[3];

	irsdkCVar cvar_car_position;

	void _fill_drivers_in_team();
	void _fill_teams();
	void _fill_session();

public:
	KevS_iRacing_Collector();
	~KevS_iRacing_Collector() = default;

	bool is_connected() override;
	void get_connection_status() override;
	int get_amnt_of_teams() override;
};

#endif // KEVS_IRACING_COLLECTOR_H