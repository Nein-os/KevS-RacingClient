#ifndef SESSION_H
#define SESSION_H

#include "./team.h"

class KevS_Session
{
public:
	KevS_Session();
	~KevS_Session() = default;

private:
	uint64_t id, track_id;
	KevS_Team *teams[MAX_TEAMS_IN_SESSION];
	unsigned short team_pos[MAX_TEAMS_IN_SESSION];
	bool b_initialized;

public:
	KevS_Team* get_team(int idx);
	bool is_same_session(int);
	bool is_initialized() const { return b_initialized; }
	uint64_t get_track_id() const { return track_id; }

	void init(int id, int track_id);
};
#endif // SESSION_H