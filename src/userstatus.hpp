#include <set>
#include <map>

using namespace std;

class UserStatus {
private:
	string name_, email_, password_;
	uint32_t id_, room_id_;
	bool is_login_, is_in_room_;

public:
	static set<string> name_set_, email_set_;
	static map<string, uint32_t> name_id_;

	UserStatus(string name, string email, string password, uint32_t id);
	string GetName(void);
	string GetEmail(void);
	bool MatchPassword(string password);
	void Login(void);
	void Logout(void);
	bool IsLogin(void);
	void SetRoom(uint32_t room_id);
	void UnsetRoom(void);
	bool IsInRoom(void);
	uint32_t GetRoomId(void);
};
