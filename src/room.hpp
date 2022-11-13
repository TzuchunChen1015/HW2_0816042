#include <set>
#include <map>

using namespace std;

class Room {
public:
	static set<unsigned int> room_id_set_;
};

class PublicRoom {
private:
	unsigned int room_id_, room_idx_;
	bool is_start_;

public:
	static set<unsigned int> room_id_set_;
	static map<unsigned int, unsigned int> room_idx_map_;

	PublicRoom(unsigned int room_id, unsigned int room_idx);
	bool IsStart(void);
};

class PrivateRoom {
private:
	unsigned int room_id_, room_idx_;
	bool is_start_;

public:
	static set<unsigned int> room_id_set_;
	static map<unsigned int, unsigned int> room_idx_map_;

	PrivateRoom(unsigned int room_id, unsigned int room_idx);
	bool IsStart(void);
};
