#include <set>
#include <map>

using namespace std;

class Room {
public:
	static set<unsigned int> room_id_set_;
};

class PublicRoom {
private:
	unsigned int room_id_, room_idx_, manager_;
	bool is_start_;

public:
	static set<unsigned int> room_id_set_;
	static map<unsigned int, unsigned int> room_idx_map_;

	PublicRoom(unsigned int room_id, unsigned int room_idx, unsigned int manager);
	bool IsStart(void);
	set<unsigned int> FD_member_; // store file descriptor;
	void JoinRoom(unsigned int fd);
	unsigned int GetManager(void);
};

class PrivateRoom {
private:
	unsigned int room_id_, room_idx_, manager_, invitation_code_;
	bool is_start_;

public:
	static set<unsigned int> room_id_set_;
	static map<unsigned int, unsigned int> room_idx_map_;

	PrivateRoom(unsigned int room_id, unsigned int room_idx, unsigned int manager, unsigned int invitation_code);
	set<unsigned int> FD_member_; // store file descriptor;
	bool IsStart(void);
	void JoinRoom(unsigned int fd);
	unsigned int GetManager(void);
	unsigned int GetInvitationCode(void);
};
