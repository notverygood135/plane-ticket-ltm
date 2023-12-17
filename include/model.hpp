#include <string>
#include <vector>
using namespace std;

vector<string> get_tickets();
vector<string> get_ticket(string id);
vector<string> create_ownership(string _username, string _ticket_id, string _full_name, string _number, string _security_code, string _expiration_date);