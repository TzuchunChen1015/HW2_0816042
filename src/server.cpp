#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <vector>
#include <sstream>
#include <algorithm>

#include "userstatus.hpp"
#include "room.hpp"

using namespace std;

const int MXL = 1e3, MXFD = 1e3;
char BUF[MXL + 1];
unsigned int FD_login_user[MXFD + 1];
vector<UserStatus> user_status;
vector<PublicRoom> public_room;
vector<PrivateRoom> private_room;

void Init(void);
void GetMessageVector(string&, vector<string>&);
string IntToString(unsigned int);
unsigned int StringToInt(string);
// TCP functions
void ProcessMessage(unsigned int, string&);
void SendMessage(unsigned int, string);
void Login(unsigned int, vector<string>&);
void Logout(unsigned int);
void CreateRoom(unsigned int, vector<string>&);
void JoinRoom(unsigned int, vector<string>&);
// UDP functions
void ProcessMessageUDP(unsigned int, string&, sockaddr_in);
void SendMessageUDP(unsigned int, string, sockaddr_in);
void Register(unsigned int, vector<string>&, sockaddr_in);
void ListRooms(unsigned int, sockaddr_in);
void ListUsers(unsigned int, sockaddr_in);

int main(int argc, char** argv) {
	Init();
	const string PORT = "5001";
	
	struct sockaddr_in server_addr, client_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(PORT.c_str()));

	unsigned int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(tcp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	listen(tcp_fd, SOMAXCONN);

	unsigned int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
	bind(udp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

	fd_set read_set, all_set;
	unsigned int max_fd = max(tcp_fd, udp_fd) + 1;
	FD_ZERO(&all_set);
	FD_SET(tcp_fd, &all_set);
	FD_SET(udp_fd, &all_set);
	
	while(true) {
		read_set = all_set;
		select(max_fd, &read_set, NULL, NULL, NULL);
		
		if(FD_ISSET(tcp_fd, &read_set)) {
			unsigned int new_fd = accept(tcp_fd, NULL, NULL);
			FD_SET(new_fd, &all_set);
			max_fd = max(max_fd, new_fd + 1);
			
		}
		
		if(FD_ISSET(udp_fd, &read_set)) {
			bzero(&client_addr, sizeof(client_addr));
			unsigned int n_bytes;
			socklen_t len = sizeof(client_addr);
			
			memset(BUF, 0, sizeof(BUF));
			n_bytes = recvfrom(udp_fd, BUF, sizeof(BUF), 0, (struct sockaddr*) &client_addr, &len);
			BUF[n_bytes] = '\0';
			string message = BUF;
			ProcessMessageUDP(udp_fd, message, client_addr);
		}

		for(unsigned int i = 5; i < max_fd; i++) {
			if(FD_ISSET(i, &read_set)) {
				unsigned int n_bytes;
				memset(BUF, 0, sizeof(BUF));
				n_bytes = recv(i, BUF, sizeof(BUF), 0);
				if(!n_bytes) {
					FD_CLR(i, &all_set);
					if(FD_login_user[i] != -1) {
						user_status[FD_login_user[i]].UnsetRoom();
						user_status[FD_login_user[i]].Logout();
						FD_login_user[i] = -1;
					}
				}
				else {
					BUF[n_bytes] = '\0';
					string message = BUF;
					ProcessMessage(i, message);
				}
			}
		}
	}
	return 0;
}

void Init(void) {
	for(unsigned int i = 0; i < MXFD; i++) {
		FD_login_user[i] = -1;
	}
	user_status.clear();
	UserStatus::name_set_.clear();
	UserStatus::email_set_.clear();
	UserStatus::name_idx_.clear();
}

void GetMessageVector(string& message, vector<string>& v) {
	stringstream ss;
	ss << message;
	string args;
	v.clear();
	while(ss >> args) {
		v.push_back(args);
	}
}

string IntToString(unsigned int num) {
	stringstream ss;
	ss << num;
	string str;
	ss >> str;
	return str;
}

unsigned int StringToInt(string str) {
	stringstream ss;
	ss << str;
	unsigned int num;
	ss >> num;
	return num;
}

void ProcessMessage(unsigned int fd, string& message) {
	vector<string> v;
	GetMessageVector(message, v);
	if(v.empty()) {}
	else if(v.front() == "login") {
		Login(fd, v);
	}
	else if(v.front() == "logout") {
		Logout(fd);
	}
	else if(v.front() == "create") { // public and private
		CreateRoom(fd, v);
	}
	else if(v.front() == "join") {
		JoinRoom(fd, v);
	}
	else if(v.front() == "invite") {

	}
	else if(v.front() == "list") {

	}
	else if(v.front() == "accept") {

	}
	else {}
}

void SendMessage(unsigned int fd, string message) {
	memset(BUF, 0, sizeof(BUF));
	memcpy(BUF, message.c_str(), message.length());
	send(fd, BUF, message.length(), 0);
	cout << "send: " << message;
}

void Login(unsigned int fd, vector<string>& v) {
	string name = v[1], password = v[2];
	if(UserStatus::name_set_.find(name) == UserStatus::name_set_.end()) {
		SendMessage(fd, "Username not found\n");
	}
	else if(FD_login_user[fd] != -1) {
		SendMessage(fd, "You already logged in as " + user_status[FD_login_user[fd]].GetName() + "\n");
	}
	else if(user_status[UserStatus::name_idx_[name]].IsLogin()) {
		SendMessage(fd, "Someone already logged in as " + name + "\n");
	}
	else if(!user_status[UserStatus::name_idx_[name]].MatchPassword(password)) {
		SendMessage(fd, "Wrong Password\n");
	}
	else {
		unsigned int idx = UserStatus::name_idx_[name];
		FD_login_user[fd] = idx;
		user_status[idx].Login();
		SendMessage(fd, "Welcome, " + name + "\n");
	}
}

void Logout(unsigned int fd) {
	if(FD_login_user[fd] == -1) {
		SendMessage(fd, "You are not logged in\n");
	}
	else if(user_status[FD_login_user[fd]].IsInRoom()) {
		SendMessage(fd, "You are already in game room " + IntToString(user_status[FD_login_user[fd]].GetRoomId()) + ", please leave game room\n");
	}
	else {
		user_status[FD_login_user[fd]].Logout();
		SendMessage(fd, "Goodbye, " + user_status[FD_login_user[fd]].GetName() + "\n");
		FD_login_user[fd] = -1;
	}
}

void CreateRoom(unsigned int fd, vector<string>& v) {
	if(FD_login_user[fd] == -1) {
		SendMessage(fd, "You are not logged in\n");
	}
	else if(user_status[FD_login_user[fd]].IsInRoom()) {
		SendMessage(fd, "You are already in game room " + IntToString(user_status[FD_login_user[fd]].GetRoomId()) + ", please leave game room\n");
	}
	else if(Room::room_id_set_.find(StringToInt(v[3])) != Room::room_id_set_.end()) {
		SendMessage(fd, "Game room ID is used, choose another one\n");
	}
	else {
		unsigned int room_id = StringToInt(v[3]);
		if(v[1] == "public") {
			public_room.push_back(PublicRoom(room_id, (unsigned int) public_room.size()));
			SendMessage(fd, "You create public game room " + v[3] + "\n");
		}
		else if(v[1] == "private") {
			private_room.push_back(PrivateRoom(room_id, (unsigned int) private_room.size()));
			SendMessage(fd, "You create private game room " + v[3] + "\n");
		}
	}
}

void JoinRoom(unsigned int fd, vector<string>& v) {
	if(FD_login_user[fd] == -1) {
		SendMessage(fd, "You are not logged in\n");
	}
	else if(user_status[FD_login_user[fd]].IsInRoom()) {
		SendMessage(fd, "You are already in game room " + IntToString(user_status[FD_login_user[fd]].GetRoomId()) + ", please leave game room\n");
	}
	else if(Room::room_id_set_.find(StringToInt(v[2])) == Room::room_id_set_.end()) {
		SendMessage(fd, "Game room " + v[2] + " is not exist\n");
	}
	else if(PrivateRoom::room_id_set_.find(StringToInt(v[2])) != PrivateRoom::room_id_set_.end()) {
		SendMessage(fd, "Game room is private, please join game by invitation code\n");
	}
	else if(public_room[PublicRoom::room_idx_map_[StringToInt(v[2])]].IsStart()) {
		SendMessage(fd, "Game started, you can't join now\n");
	}
	else {
		unsigned int idx = FD_login_user[fd];
		unsigned int room_idx = PublicRoom::room_idx_map_[StringToInt(v[2])];
		user_status[idx].SetRoom(StringToInt(v[2]));
		for(set<unsigned int>::iterator it = public_room[room_idx].FD_member_.begin(); it != public_room[room_idx].FD_member_.end(); it++) {
			SendMessage(*it, "Welcome, " + user_status[idx].GetName() + " to game!\n");
		}
		public_room[room_idx].JoinRoom(fd);
		SendMessage(fd, "You join game room " + v[2] + "\n");
	}
}

void ProcessMessageUDP(unsigned int fd, string& message, sockaddr_in client_addr) {
	cout << "receive: " << message;
	vector<string> v;
	GetMessageVector(message, v);
	if(v.empty()) {}
	else if(v.front() == "register") {
		Register(fd, v, client_addr);
	}
	else if(message == "list rooms\n") {
		ListRooms(fd, client_addr);
	}
	else if(message == "list users\n") {
		ListUsers(fd, client_addr);
	}
	else {
		SendMessageUDP(fd, "Command Not Found\n", client_addr);
	}
}

void SendMessageUDP(unsigned int fd, string message, sockaddr_in client_addr) {
	memset(BUF, 0, sizeof(BUF));
	memcpy(BUF, message.c_str(), message.length());
	sendto(fd, BUF, message.length(), 0, (const struct sockaddr*) &client_addr, sizeof(client_addr));
}

void Register(unsigned int fd, vector<string>& v, sockaddr_in client_addr) {
	string name = v[1], email = v[2], password = v[3];
	if(UserStatus::name_set_.find(name) != UserStatus::name_set_.end() || UserStatus::email_set_.find(email) != UserStatus::email_set_.end()) {
		SendMessageUDP(fd, "Username or Email is already used\n", client_addr);
	}
	else {
		user_status.push_back(UserStatus(name, email, password, (unsigned int) user_status.size()));
		SendMessageUDP(fd, "Register Successfully\n", client_addr);
	}
}

void ListRooms(unsigned int fd, sockaddr_in client_addr) {
	string message;
	if(Room::room_id_set_.empty()) {
		message = "No Rooms\n";
	}
	else {
		set<unsigned int>::iterator it;
		unsigned int idx = 1;
		for(it = Room::room_id_set_.begin(); it != Room::room_id_set_.end(); it++, idx++) {
			message += IntToString(idx) + ".";
			unsigned int room_id = *it;
			if(PublicRoom::room_id_set_.find(room_id) != PublicRoom::room_id_set_.end()) {
				message += "(Public) ";
				message += "Game Room " + IntToString(room_id) + " ";
				unsigned room_idx = PublicRoom::room_idx_map_[room_id];
				if(public_room[room_idx].IsStart()) {
					message += "has started playing\n";
				}
				else {
					message += "is open for players\n";
				}
			}
			else if(PrivateRoom::room_id_set_.find(room_id) != PrivateRoom::room_id_set_.end()) {
				message += "(Private) ";
				message += "Game Room " + IntToString(room_id) + " ";
				unsigned room_idx = PrivateRoom::room_idx_map_[room_id];
				if(private_room[room_idx].IsStart()) {
					message += "has started playing\n";
				}
				else {
					message += "is open for players\n";
				}
			}
		}
	}
	SendMessageUDP(fd, message, client_addr);
}

void ListUsers(unsigned int fd, sockaddr_in client_addr) {
	string message;
	if(user_status.empty()) {
		message = "No Users\n";
	}
	else {
		vector<string> v;
		for(unsigned int i = 0; i < user_status.size(); i++) {
			string str;
			str += user_status[i].GetName();
			str += "<" + user_status[i].GetEmail() + "> ";
			if(user_status[i].IsLogin()) {
				str += "Online\n";
			}
			else {
				str += "Offline\n";
			}
			v.push_back(str);
		}
		sort(v.begin(), v.end());
		for(unsigned int i = 0; i < v.size(); i++) {
			message += IntToString(i + 1) + ".\n";
			message += v[i];
		}
	}
	SendMessageUDP(fd, message, client_addr);
}
