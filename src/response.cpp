#include "../include/response.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

string render_static_file(string fileName) {
	string response_data = "";
	string buff = "";
	ifstream readf(fileName);

	while (readf.good()){
		buff = readf.get();
		response_data.append(buff);
	}

	return response_data;
}
