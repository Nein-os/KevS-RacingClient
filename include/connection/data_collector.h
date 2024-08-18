#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

class KevS_Session;

class IKevS_DataCollector
{
public:
	IKevS_DataCollector();
	virtual ~IKevS_DataCollector();
	
private:
	virtual void _fill_drivers_in_team() = 0;
	virtual void _fill_teams() = 0;
	virtual void _fill_session() = 0;

protected:
	KevS_Session *current_session;

public:
	virtual bool is_connected() = 0;
	virtual void get_connection_status() = 0; // TODO: currently no use
	int get_amnt_of_teams();
	virtual void update_session() = 0;
	virtual void reset_session();
	
	virtual float get_car_pos(int) = 0;
	virtual int get_overall_place(int) = 0;
	virtual char *get_number(int) = 0;
	virtual int get_class_place(int) = 0;
	virtual int get_class_colour_r(int) = 0;
	virtual int get_class_colour_g(int) = 0;
	virtual int get_class_colour_b(int) = 0;
	virtual int get_class(int) = 0;
	virtual int get_own_class() = 0;
	virtual float get_own_track_pos() = 0;

	virtual bool is_on_pitroad(int) = 0;
	virtual float get_best_lap_time(int) = 0;
	virtual float get_self_best_lap_time() = 0;
};
#endif // DATA_COLLECTOR_H