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
void PublicRoom::JoinRoom(unsigned int fd) { this->FD_member_.push_back(fd); }
void PublicRoom::LeaveRoom(unsigned int fd) {
	for(vector<unsigned int>::iterator it = this->FD_member_.begin(); it != this->FD_member_.end(); it++) {
		if(*it == fd) {
			this->FD_member_.erase(it);
			break;
		}
	}
}
unsigned int PublicRoom::GetManager(void) { return this->manager_; }
void PublicRoom::StartGame(unsigned int round, string number) {
	this->is_start_ = true;
	this->round_ = 0;
	this->mx_round_ = round * (unsigned int) this->FD_member_.size();
	this->number_ = number;
	this->current_player_ = this->FD_member_.front();
}
void PublicRoom::ResetGame(void) { this->is_start_ = false; }
bool PublicRoom::MatchCurrentPlayer(unsigned int fd) { return (this->current_player_ == fd); }
unsigned int PublicRoom::GetCurrentPlayer(void) { return this->current_player_; }
string PublicRoom::GetNumber(void) { return this->number_; }
void PublicRoom::NextRound(void) {
	this->round_++;
	this->current_player_ = this->FD_member_[this->round_ % (unsigned int) (this->FD_member_.size())];
}
bool PublicRoom::EndTheGame(void) { return (this->round_ >= this->mx_round_); }

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
void PrivateRoom::JoinRoom(unsigned int fd) { this->FD_member_.push_back(fd); }
void PrivateRoom::LeaveRoom(unsigned int fd) {
	for(vector<unsigned int>::iterator it = this->FD_member_.begin(); it != this->FD_member_.end(); it++) {
		if(*it == fd) {
			this->FD_member_.erase(it);
			break;
		}
	}
}
unsigned int PrivateRoom::GetManager(void) { return this->manager_; }
unsigned int PrivateRoom::GetInvitationCode(void) { return this->invitation_code_; };
bool PrivateRoom::MatchInvitationCode(unsigned int invitation_code) { return (this->invitation_code_ == invitation_code); }
void PrivateRoom::StartGame(unsigned int round, string number) {
	this->is_start_ = true;
	this->round_ = 0;
	this->mx_round_ = this->round_ * (unsigned int) this->FD_member_.size();
	this->number_ = number;
	this->current_player_ = this->FD_member_.front();
}
void PrivateRoom::ResetGame(void) { this->is_start_ = false; }
bool PrivateRoom::MatchCurrentPlayer(unsigned int fd) { return (this->current_player_ == fd); }
unsigned int PrivateRoom::GetCurrentPlayer(void) { return this->current_player_; }
string PrivateRoom::GetNumber(void) { return this->number_; }
void PrivateRoom::NextRound(void) {
	this->round_++;
	this->current_player_ = this->FD_member_[this->round_ % (unsigned int) (this->FD_member_.size())];
}
bool PrivateRoom::EndTheGame(void) { return (this->round_ >= this->mx_round_); }
