#include "datatypes/team.h"

KevS_Team::KevS_Team()
{
	current_driver = -1;
	b_initialized = false;
	name = "";
}

void KevS_Team::init()
{
	b_initialized = true;
}