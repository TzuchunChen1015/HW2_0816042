#include "userstatus.hpp"

set<string> UserStatus::name_set_;
set<string> UserStatus::email_set_;
map<string, uint32_t> UserStatus::name_id_;

UserStatus::UserStatus(string name, string email, string password, uint32_t id) {
	this->name_ = name;
	this->email_ = email;
	this->password_ = password;
	this->id_ = id;
	this->is_login_ = this->is_in_room_ = false;

	name_set_.insert(name);
	email_set_.insert(email);
	name_id_[name] = id;
}
string UserStatus::GetName(void) { return this->name_; }
string UserStatus::GetEmail(void) { return this->email_; }
bool UserStatus::MatchPassword(string password) { return (password == this->password_); }
void UserStatus::Login(void) { this->is_login_ = true; }
void UserStatus::Logout(void) { this->is_login_ = false; }
bool UserStatus::IsLogin(void) { return this->is_login_; }
void UserStatus::SetRoom(uint32_t room_id) {
	this->room_id_ = room_id;
	this->is_in_room_ = true;
}
void UserStatus::UnsetRoom(void) { this->is_in_room_ = false; }
bool UserStatus::IsInRoom(void) { return this->is_in_room_; }
uint32_t UserStatus::GetRoomId(void) { return this->room_id_; }
