#include "room.hpp"

// For Class Room
set<unsigned int> Room::room_id_set_;

// For Class PublicRoom
set<unsigned int> PublicRoom::room_id_set_;
map<unsigned int, unsigned int> PublicRoom::room_idx_map_;

PublicRoom::PublicRoom(unsigned int room_id, unsigned int room_idx, unsigned int manager) {
	this->room_id_ = room_id;
	this->room_idx_ = room_idx;
	this->manager_ = manager;
	this->is_start_ = false;
	this->FD_member_.clear();

	Room::room_id_set_.insert(room_id);
	room_id_set_.insert(room_id);
	room_idx_map_[room_id] = room_idx;
}
bool PublicRoom::IsStart(void) { return this->is_start_; }
void PublicRoom::JoinRoom(unsigned int fd) { this->FD_member_.insert(fd); }
unsigned int PublicRoom::GetManager(void) { return this->manager_; }

// For Class PrivateRoom
set<unsigned int> PrivateRoom::room_id_set_;
map<unsigned int, unsigned int> PrivateRoom::room_idx_map_;

PrivateRoom::PrivateRoom(unsigned int room_id, unsigned int room_idx, unsigned int manager, unsigned int invitation_code) {
	this->room_id_ = room_id;
	this->room_idx_ = room_idx;
	this->manager_ = manager;
	this->invitation_code_ = invitation_code;
	this->is_start_ = false;
	this->FD_member_.clear();

	Room::room_id_set_.insert(room_id);
	room_id_set_.insert(room_id);
	room_idx_map_[room_id] = room_idx;
}
bool PrivateRoom::IsStart(void) { return this->is_start_; }
void PrivateRoom::JoinRoom(unsigned int fd) { this->FD_member_.insert(fd); }
unsigned int PrivateRoom::GetManager(void) { return this->manager_; }
unsigned int PrivateRoom::GetInvitationCode(void) { return this->invitation_code_; };
bool PrivateRoom::MatchInvitationCode(unsigned int invitation_code) { return (this->invitation_code_ == invitation_code); }
