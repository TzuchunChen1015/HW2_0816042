#include <set>
#include <map>
#include <vector>
#include <string>

using namespace std;

class Room {
public:
	static set<unsigned int> room_id_set_;
};

class PublicRoom {
private:
	unsigned int room_id_, room_idx_, manager_, round_, mx_round_, current_player_;
	bool is_start_;
	string number_;

public:
	static set<unsigned int> room_id_set_;
	static map<unsigned int, unsigned int> room_idx_map_;

	PublicRoom(unsigned int room_id, unsigned int room_idx, unsigned int manager);
	bool IsStart(void);
	vector<unsigned int> FD_member_; // store file descriptor;
	void JoinRoom(unsigned int fd);
	void LeaveRoom(unsigned int fd);
	unsigned int GetManager(void);
	void StartGame(unsigned int round, string number);
	void ResetGame(void);
	bool MatchCurrentPlayer(unsigned int fd);
	unsigned int GetCurrentPlayer(void);
	string GetNumber(void);
	void NextRound(void);
	bool EndTheGame(void);
};

class PrivateRoom {
private:
	unsigned int room_id_, room_idx_, manager_, invitation_code_, round_, mx_round_, current_player_;
	bool is_start_;
	string number_;

public:
	static set<unsigned int> room_id_set_;
	static map<unsigned int, unsigned int> room_idx_map_;

	PrivateRoom(unsigned int room_id, unsigned int room_idx, unsigned int manager, unsigned int invitation_code);
	vector<unsigned int> FD_member_; // store file descriptor;
	bool IsStart(void);
	void JoinRoom(unsigned int fd);
	void LeaveRoom(unsigned int fd);
	unsigned int GetManager(void);
	unsigned int GetInvitationCode(void);
	bool MatchInvitationCode(unsigned int invitation_code);
	void StartGame(unsigned int round, string number);
	void ResetGame(void);
	bool MatchCurrentPlayer(unsigned int fd);
	unsigned int GetCurrentPlayer(void);
	string GetNumber(void);
	void NextRound(void);
	bool EndTheGame(void);
};
