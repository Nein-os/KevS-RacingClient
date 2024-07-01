#ifndef IKEVS_DATASYNC_H
#define IKEVS_DATASYNC_H

class IKevS_DataSync
{
public:
	virtual void is_connected() = 0;
	virtual void send_data() = 0;
};

#endif // IKEVS_DATASYNC_H