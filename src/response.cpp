#include "../include/response.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

string render_static_file(string fileName) {
	// FILE* file = fopen(fileName.c_str(), "r");

	// if (file == NULL) {
	// 	return NULL;
	// }else {
	// 	cout << fileName << " does exist" << endl;
	// }

	// fseek(file, 0, SEEK_END);
	// long fsize = ftell(file);
	// fseek(file, 0, SEEK_SET);

	// char* response_data = (char*) malloc(sizeof(char) * (fsize+1));
	// char ch;
	// int i = 0;
	// while((ch = fgetc(file)) != EOF) {
	// 	response_data[i] = ch;
	// 	i++;
	// }
	// fclose(file);

	string response_data = "";
	string buff = "";
	ifstream readf(fileName);

	while (readf.good()){
		buff = readf.get();
		response_data.append(buff);
	}

	return response_data;
}
