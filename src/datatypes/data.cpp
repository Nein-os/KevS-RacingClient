#include "datatypes/data.h"
#include "connection/iracing_collector.h"

IKevS_DataCollector *KevS_Data::collector = nullptr;

KevS_Data::KevS_Data()
{
	collector = new KevS_iRacing_Collector();
}

bool KevS_Data::is_session_connected()
{
	if (collector)
		return collector->is_connected();
	return false;
}

int KevS_Data::get_amount_of_teams()
{
	return collector->get_amnt_of_teams();
}