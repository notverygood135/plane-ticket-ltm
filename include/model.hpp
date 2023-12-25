#include <string>
#include <vector>
using namespace std;

vector<string> get_flights();
vector<string> get_tickets(string flight_id);
vector<string> get_owned_tickets(string username);
vector<string> get_ticket(string ticket_id);
vector<string> create_ownership(string _username, string _ticket_id, string _full_name, string _number, string _security_code, string _expiration_date);
vector<string> delete_ownership(string _ticket_id);