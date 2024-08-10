#ifndef TEAM_H
#define TEAM_H

#include "driver.h"
#include "../defines.h"

class KevS_Team
{
public:
	KevS_Team();
	~KevS_Team() = default;

	void init(std::string name, int, int, int, char[]);

private:
	short current_driver;
	bool b_initialized;
	std::string name;
	KevS_Driver *drivers[MAX_DRIVERS_IN_TEAM];
	int red, green, blue;
	char car_number[4];

public:
	bool is_initialized() const { return b_initialized; }
	int get_overall_pos();
	void add_driver(uint64_t idx, std::string name);
	int get_red() const { return red; }
	int get_green() const { return green; }
	int get_blue() const { return blue; }
	char *get_car_number() { return car_number; };
};
#endif // TEAM_H