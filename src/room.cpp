#include "room.hpp"

// For Class Room
set<unsigned int> Room::room_id_set_;

// For Class PublicRoom
set<unsigned int> PublicRoom::room_id_set_;
map<unsigned int, unsigned int> PublicRoom::room_idx_map_;

PublicRoom::PublicRoom(unsigned int room_id, unsigned int room_idx) {
	this->room_id_ = room_id;
	this->room_idx_ = room_idx;
	this->is_start_ = false;
	this->FD_member_.clear();

	Room::room_id_set_.insert(room_id);
	room_id_set_.insert(room_id);
	room_idx_map_[room_id] = room_idx;
}
bool PublicRoom::IsStart(void) { return this->is_start_; }
void PublicRoom::JoinRoom(unsigned int fd) { this->FD_member_.insert(fd); }

// For Class PrivateRoom
set<unsigned int> PrivateRoom::room_id_set_;
map<unsigned int, unsigned int> PrivateRoom::room_idx_map_;

PrivateRoom::PrivateRoom(unsigned int room_id, unsigned int room_idx) {
	this->room_id_ = room_id;
	this->room_idx_ = room_idx;
	this->is_start_ = false;
	this->FD_member_.clear();

	Room::room_id_set_.insert(room_id);
	room_id_set_.insert(room_id);
	room_idx_map_[room_id] = room_idx;
}
bool PrivateRoom::IsStart(void) { return this->is_start_; }
