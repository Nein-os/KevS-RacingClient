#include "datatypes/team.h"

KevS_Team::KevS_Team()
{
	current_driver = -1;
	b_initialized = false;
	name = "";
	for (int i = 0; i < MAX_DRIVERS_IN_TEAM; i++) 
		drivers[i] = new KevS_Driver();
	red = green = blue = 0;
	car_number[0] = '#';
	for (int i = 1; i < 4; i++)
		car_number[i] = 0;
}

void KevS_Team::init(std::string name, int red, int green, int blue, char car_number[])
{
	this->name = name;
	b_initialized = true;
	this->red = red;
	this->green = green;
	this->blue = blue;
	for (int i = 1; i < 4; i++)
		this->car_number[i] = car_number[i-1];
}

void KevS_Team::add_driver(uint64_t idx, std::string name)
{
	int i = 0;
	while (drivers[i]->is_initialized())
		if (drivers[i]->get_driver_id() == idx)
			return;
		i++;
	drivers[i] = new KevS_Driver(idx, name);
}