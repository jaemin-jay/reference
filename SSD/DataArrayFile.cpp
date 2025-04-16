#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class DataArrayFile {
private:
	string filePath;
public:
	DataArrayFile(string filePath)
		: filePath(filePath) {

	}

	bool isCreatedFile() {
		ifstream inFile(filePath);
		bool ret = inFile.is_open();
		inFile.close();
		return ret;
	}

	int readFileLines(string buf[], int size) {
		ifstream inFile(filePath);
		int cnt = 0;
		for (int i = 0; i < size; ++i) {
			if (!getline(inFile, buf[i]) || buf[i] == "")
				break;
			cnt++;
		}
		inFile.close();
		return cnt;
	}

	void writeFileLines(const string buf[], int size) {
		ofstream outFile(filePath);
		for (int i = 0; i < size; ++i) {
			outFile << buf[i] << endl;
		}
		outFile.close();
	}
};
