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

using namespace std;

const int MXL = 1e3, MXFD = 1e3;
char BUF[MXL + 1];
uint32_t FD_login_user[MXFD + 1];
vector<UserStatus> user_status;

void Init(void);
void GetMessageVector(string&, vector<string>&);
string IntToString(uint32_t);
uint32_t StringToInt(string);
// TCP functions
void ProcessMessage(uint32_t, string&);
void SendMessage(uint32_t, string);
void Login(uint32_t, vector<string>&);
void Logout(uint32_t);
// UDP functions
void ProcessMessageUDP(uint32_t, string&, sockaddr_in);
void SendMessageUDP(uint32_t, string, sockaddr_in);
void Register(uint32_t, vector<string>&, sockaddr_in);
void ListUsers(uint32_t, sockaddr_in);

int main(int argc, char** argv) {
	Init();
	const string PORT = "5001";
	
	struct sockaddr_in server_addr, client_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(PORT.c_str()));

	uint32_t tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(tcp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	listen(tcp_fd, SOMAXCONN);

	uint32_t udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
	bind(udp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

	fd_set read_set, all_set;
	uint32_t max_fd = max(tcp_fd, udp_fd) + 1;
	FD_ZERO(&all_set);
	FD_SET(tcp_fd, &all_set);
	FD_SET(udp_fd, &all_set);
	
	while(true) {
		read_set = all_set;
		select(max_fd, &read_set, NULL, NULL, NULL);
		
		if(FD_ISSET(tcp_fd, &read_set)) {
			uint32_t new_fd = accept(tcp_fd, NULL, NULL);
			FD_SET(new_fd, &all_set);
			max_fd = max(max_fd, new_fd + 1);
			
		}
		
		if(FD_ISSET(udp_fd, &read_set)) {
			bzero(&client_addr, sizeof(client_addr));
			uint32_t n_bytes;
			socklen_t len = sizeof(client_addr);
			
			memset(BUF, 0, sizeof(BUF));
			n_bytes = recvfrom(udp_fd, BUF, sizeof(BUF), 0, (struct sockaddr*) &client_addr, &len);
			BUF[n_bytes] = '\0';
			string message = BUF;
			ProcessMessageUDP(udp_fd, message, client_addr);
		}

		for(uint32_t i = 5; i < max_fd; i++) {
			if(FD_ISSET(i, &read_set)) {
				uint32_t n_bytes;
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
	for(uint32_t i = 0; i < MXFD; i++) {
		FD_login_user[i] = -1;
	}
	user_status.clear();
	UserStatus::name_set_.clear();
	UserStatus::email_set_.clear();
	UserStatus::name_id_.clear();
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

string IntToString(uint32_t num) {
	stringstream ss;
	ss << num;
	string str;
	ss >> str;
	return str;
}

uint32_t StringToInt(string str) {
	stringstream ss;
	ss << str;
	uint32_t num;
	ss >> num;
	return num;
}

void ProcessMessage(uint32_t fd, string& message) {
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

	}
	else if(v.front() == "join") {

	}
	else if(v.front() == "invite") {

	}
	else if(v.front() == "list") {

	}
	else if(v.front() == "accept") {

	}
	else {}
}

void SendMessage(uint32_t fd, string message) {
	memset(BUF, 0, sizeof(BUF));
	memcpy(BUF, message.c_str(), message.length());
	send(fd, BUF, message.length(), 0);
	cout << "send: " << message;
}

void Login(uint32_t fd, vector<string>& v) {
	string name = v[1], password = v[2];
	if(UserStatus::name_set_.find(name) == UserStatus::name_set_.end()) {
		SendMessage(fd, "Username not found\n");
	}
	else if(FD_login_user[fd] != -1) {
		SendMessage(fd, "You already logged in as " + user_status[FD_login_user[fd]].GetName() + "\n");
	}
	else if(user_status[UserStatus::name_id_[name]].IsLogin()) {
		SendMessage(fd, "Someone already logged in as " + name + "\n");
	}
	else if(!user_status[UserStatus::name_id_[name]].MatchPassword(password)) {
		SendMessage(fd, "Wrong Password\n");
	}
	else {
		uint32_t id = UserStatus::name_id_[name];
		FD_login_user[fd] = id;
		user_status[id].Login();
		SendMessage(fd, "Welcome, " + name + "\n");
	}
}

void Logout(uint32_t fd) {
	if(FD_login_user[fd] == -1) {
		SendMessage(fd, "You are not logged in\n");
	}
	else if(user_status[FD_login_user[fd]].IsInRoom()) {
		uint32_t room_id = user_status[FD_login_user[fd]].GetRoomId();
		string str = IntToString(room_id);
		SendMessage(fd, "You are already in game room " + str + ", please leave game room\n");
	}
	else {
		user_status[FD_login_user[fd]].Logout();
		SendMessage(fd, "Goodbye, " + user_status[FD_login_user[fd]].GetName() + "\n");
		FD_login_user[fd] = -1;
	}
}

void ProcessMessageUDP(uint32_t fd, string& message, sockaddr_in client_addr) {
	cout << "receive: " << message;
	vector<string> v;
	GetMessageVector(message, v);
	if(v.empty()) {}
	else if(v.front() == "register") {
		Register(fd, v, client_addr);
	}
	else if(message == "list rooms\n") {

	}
	else if(message == "list users\n") {
		ListUsers(fd, client_addr);
	}
	else {
		SendMessageUDP(fd, "Command Not Found\n", client_addr);
	}
}

void SendMessageUDP(uint32_t fd, string message, sockaddr_in client_addr) {
	memset(BUF, 0, sizeof(BUF));
	memcpy(BUF, message.c_str(), message.length());
	sendto(fd, BUF, message.length(), 0, (const struct sockaddr*) &client_addr, sizeof(client_addr));
}

void Register(uint32_t fd, vector<string>& v, sockaddr_in client_addr) {
	string name = v[1], email = v[2], password = v[3];
	if(UserStatus::name_set_.find(name) != UserStatus::name_set_.end() || UserStatus::email_set_.find(email) != UserStatus::email_set_.end()) {
		SendMessageUDP(fd, "Username or Email is already used\n", client_addr);
	}
	else {
		user_status.push_back(UserStatus(name, email, password, (uint32_t) user_status.size()));
		SendMessageUDP(fd, "Register Successfully\n", client_addr);
	}
}

void ListUsers(uint32_t fd, sockaddr_in client_addr) {
	string message;
	if(user_status.empty()) {
		message = "No Users\n";
	}
	else {
		vector<string> v;
		for(uint32_t i = 0; i < user_status.size(); i++) {
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
		for(uint32_t i = 0; i < v.size(); i++) {
			message += IntToString(i + 1) + ".\n";
			message += v[i];
		}
	}
	SendMessageUDP(fd, message, client_addr);
}
