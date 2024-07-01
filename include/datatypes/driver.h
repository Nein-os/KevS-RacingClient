#ifndef DRIVER_H
#define DRIVER_H

#include <string>

class KevS_Driver
{
public:
	KevS_Driver();
	~KevS_Driver() = default;
private:
	uint64_t id;
	std::string driver_name;

public:
	std::string get_driver_name();
};
#endif // DRIVER_H