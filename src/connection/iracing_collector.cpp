#include "connection/iracing_collector.h"
#include <stdlib.h>

char KevS_iRacing_Collector::a_amnt_entries[3];

KevS_iRacing_Collector::KevS_iRacing_Collector() : IKevS_DataCollector()
{
	cvar_car_position = irsdkCVar("CarIdxLapDistPct");

	for (int i = 0; i < 3; i++)
		KevS_iRacing_Collector::a_amnt_entries[i] = '\0';
}

bool KevS_iRacing_Collector::is_connected() 
{
	return irsdkClient::instance().isConnected();
}

void KevS_iRacing_Collector::get_connection_status() 
{
	// TODO
}

int KevS_iRacing_Collector::get_amnt_of_teams() 
{
	// TODO
	return 0;
}