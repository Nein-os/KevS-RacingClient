#ifndef TEAM_H
#define TEAM_H

#include "driver.h"
#include "../defines.h"
#include <vector>
#include <memory>

class KevS_Team
{
public:
	KevS_Team();
	~KevS_Team() = default;

	void init();

private:
	short current_driver;
	bool b_initialized;
	std::string name;
	KevS_Driver drivers[MAX_DRIVERS_IN_TEAM];

public:
	bool is_initialized() const { return b_initialized; }
	int get_overall_pos();
};
#endif // TEAM_H