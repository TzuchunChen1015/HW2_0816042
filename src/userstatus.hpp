#include <set>
#include <map>
#include <vector>

using namespace std;

class UserStatus {
private:
	string name_, email_, password_;
	unsigned int idx_, room_id_, fd_;
	bool is_login_, is_in_room_;

public:
	static set<string> name_set_, email_set_;
	static map<string, unsigned int> name_idx_, email_idx_;

	UserStatus(string name, string email, string password, unsigned int idx);
	string GetName(void);
	string GetEmail(void);
	bool MatchPassword(string password);
	void Login(unsigned int fd);
	void Logout(void);
	bool IsLogin(void);
	void SetRoom(unsigned int room_id);
	void UnsetRoom(void);
	bool IsInRoom(void);
	unsigned int GetRoomId(void);
	unsigned int GetFD(void);
	set<unsigned int> invitation_room_id;
};
