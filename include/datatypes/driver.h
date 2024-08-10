#ifndef DRIVER_H
#define DRIVER_H

#include <string>

class KevS_Driver
{
public:
	KevS_Driver();
	KevS_Driver(uint64_t id, std::string name);
	~KevS_Driver() = default;
private:
	uint64_t id;
	std::string driver_name;
	bool b_initialized;
	void _init(uint64_t id, std::string name);

public:
	std::string get_driver_name();
	bool is_initialized() const { return b_initialized; }
	uint64_t get_driver_id() const { return id; }
};
#endif // DRIVER_H