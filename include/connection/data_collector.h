#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

class IKevS_DataCollector
{
public:
	virtual bool is_connected() = 0;
	virtual void get_connection_status() = 0; // TODO: currently no use
	virtual int get_amnt_of_teams() = 0;
};
#endif // DATA_COLLECTOR_H