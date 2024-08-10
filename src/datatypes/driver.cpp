#include "datatypes/driver.h"

KevS_Driver::KevS_Driver()
{
	id = 0;
	driver_name = "";
	b_initialized = false;
}
KevS_Driver::KevS_Driver(uint64_t id, std::string name) : KevS_Driver()
{ 
	if (id) _init(id, name); 
}

void KevS_Driver::_init(uint64_t id, std::string name)
{
	if (!b_initialized) {
		this->id = id;
		driver_name = name;
		b_initialized = true;
	}
}