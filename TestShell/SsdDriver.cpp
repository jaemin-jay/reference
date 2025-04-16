#pragma once
#include "ISSD.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

class SSD_Driver : public ISSD {
public:
	explicit SSD_Driver(string outputName) : outputName(outputName) {

	}
	string read(int lba) override {
		string cmd = "SSD.exe R " + to_string(lba);
		system(cmd.c_str());

		ifstream fp(outputName.c_str());
		string line;
		if (fp.is_open())
			getline(fp, line);

		fp.close();

		return line;
	}
	void write(int lba, string value) override {
		string cmd = "SSD.exe W " + to_string(lba) + " " + value;
		cout << cmd << endl;
		system(cmd.c_str());
	}

	string outputName;
};