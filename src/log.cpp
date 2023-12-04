#include <string>
#include <chrono>
#include <fstream>
#include "../include/utils.hpp"
using namespace std;

void log(string s) {
    ofstream file("log.txt", ios_base::app);
    const auto t = chrono::system_clock::now();
    const time_t t_time_t = chrono::system_clock::to_time_t(t);
    file << ctime(&t_time_t) << endl << s << endl;
    file.close();
}