#ifndef SESSION_H
#define SESSION_H

#include "./team.h"

class IKevS_Session
{
public:
	IKevS_Session();
	~IKevS_Session() = default;

private:
	uint64_t id;
	KevS_Team teams[MAX_TEAMS_IN_SESSION];

public:
	KevS_Team get_team(int idx);
};
#endif // SESSION_H