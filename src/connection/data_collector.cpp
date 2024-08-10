#include "connection/data_collector.h"
#include "datatypes/session.h"
#include "defines.h"
#include <cassert>

IKevS_DataCollector::IKevS_DataCollector()
{
	current_session = nullptr;
}

int IKevS_DataCollector::get_amnt_of_teams()
{
	int ret = 0;
	for (int i = 0; i < MAX_TEAMS_IN_SESSION; i++) {
		KevS_Team *team = current_session->get_team(i);
		assert(team == nullptr);
		if (team->is_initialized())
			ret++;
	}
	return ret;
}

void IKevS_DataCollector::reset_session()
{
	if (current_session)
		delete current_session;
	current_session = new KevS_Session();
	_fill_session();
}

IKevS_DataCollector::~IKevS_DataCollector()
{
	delete current_session;
}