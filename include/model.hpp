#include <string>
#include <vector>
using namespace std;

vector<string> get_flights();
vector<string> get_flight(string flight_id);
vector<string> update_flight(string _flight_id, string _date, string _time);
vector<string> delete_flight(string _flight_id);

vector<string> get_tickets(string flight_id);
vector<string> get_owned_tickets(string email);
vector<string> get_ticket(string ticket_id);

vector<string> get_ownership(string ticket_id);
vector<string> get_ownerships();
vector<string> create_ownership(string _email, string _ticket_id, string _full_name, string _number, string _security_code, string _expiration_date, string _book_date);
vector<string> delete_ownership(string _ticket_id);

vector<string> get_notifications(string email);
vector<string> get_unread_notifications_count(string email);
vector<string> create_notification(string _email, string _content, string _date, string _time);
vector<string> update_notifications(string email);
vector<string> delete_notifications(string email);

vector<string> get_user(string email);
vector<string> get_users();
vector<string> get_passengers(string flight_id);
vector<string> delete_user(string email);
vector<string> update_type(string _email, string _type);
vector<string> update_money(string _email, string _money);
vector<string> update_bonus(string _email, string _bonus);