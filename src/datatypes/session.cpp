#include "../../include/datatypes/session.h"

KevS_Session::KevS_Session()
{
	id = 0;
	track_id = 0;
	for (int i = 0; i < MAX_TEAMS_IN_SESSION; i++) {
		team_pos[i] = 0;
		teams[i] = new KevS_Team();
	}
	b_initialized = false;
}

KevS_Team* KevS_Session::get_team(int idx)
{
	KevS_Team *ret = nullptr;
	if (idx >= 0 && idx < MAX_TEAMS_IN_SESSION)
		ret = teams[idx];
	return ret;
}

void KevS_Session::init(int id, int track_id)
{
	if (id && track_id) {
		this->id = id; 
		this->track_id = track_id;
		b_initialized = true;
	}
}

bool KevS_Session::is_same_session(int id) { return this->id == id; }