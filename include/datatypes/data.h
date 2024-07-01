#ifndef KEVS_DATA_H
#define KEVS_DATA_H

class IKevS_DataCollector;

class KevS_Data
{
public:
	KevS_Data();
	~KevS_Data() = default;

private:
	static IKevS_DataCollector *collector;

public:
	static bool is_session_connected();
	static bool is_db_connected();
	
	static int get_amount_of_teams();
	static int get_own_team_idx();
};

#endif // KEVS_DATA_H