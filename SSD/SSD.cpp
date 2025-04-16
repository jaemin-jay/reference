#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "DataArrayFile.cpp"

using namespace std;

class ISSD {
public:
	virtual void read(int lba) = 0;
	virtual void write(int lba, string data) = 0;
	virtual void erase(int lba, int size) = 0;
	virtual void flush(void) = 0;
};

static const int SSD_MAX_DATA_SIZE = 100;
static const int SSD_FIXED_DATA_LENGTH = 10;
static const string SSD_DEFAULT_DATA = "0x00000000";
static const int SSD_MAX_ERASE_SIZE = 10;

class SSD : public ISSD {
private:
	string mData[SSD_MAX_DATA_SIZE];
	string writeBuffer[10];
	DataArrayFile nandFile{ "nand.txt" };
	DataArrayFile resultFile{ "result.txt" };
	DataArrayFile bufferFile{ "buffer.txt" };

	bool isInvalidData(string data) {
		return (data.length() != SSD_FIXED_DATA_LENGTH || data.substr(0, 2) != "0x");
	}

	bool isInvalidLBA(int lba) {
		return (lba < 0 || lba >= SSD_MAX_DATA_SIZE);
	}

	bool isInvalidEraseSize(int size) {
		return (size < 0 || size > SSD_MAX_ERASE_SIZE);
	}

	vector<std::string> splitString(const std::string& input, char delim) {
		vector<std::string> tokens;
		istringstream iss(input);
		string token;

		while (getline(iss, token, delim)) {
			tokens.push_back(token);
		}

		return tokens;
	}

	bool fastRead(int lba) {
		string buffer[10];
		int size = bufferFile.readFileLines(buffer, 10);
		for (int i = size - 1; i >= 0; i--) {
			vector<string> tokens = splitString(buffer[i], ' ');
			if (tokens.size() != 3) {
				throw invalid_argument("buffer has invalid data");
			}

			if (tokens[0] == "W" && stoi(tokens[1]) == lba) {
				resultFile.writeFileLines(&tokens[2], 1);
				return true;
			}
			else if (tokens[0] == "E" && lba >= stoi(tokens[1]) && lba < stoi(tokens[1]) + stoi(tokens[2])) {
				resultFile.writeFileLines(&SSD_DEFAULT_DATA, 1);
				return true;
			}
		}
		return false;
	}
public:
	SSD() {
		for (int i = 0; i < SSD_MAX_DATA_SIZE; ++i) {
			mData[i] = SSD_DEFAULT_DATA;
		}
		if (!nandFile.isCreatedFile()) {
			nandFile.writeFileLines(mData, SSD_MAX_DATA_SIZE);
		}
	}

	void read(int lba) {
		if (isInvalidLBA(lba)) {
			throw out_of_range("LBA is out of range");
		}
		if (fastRead(lba)) {
			return;
		}

		nandFile.readFileLines(mData, SSD_MAX_DATA_SIZE);
		resultFile.writeFileLines(&mData[lba], 1);
	}

	void flush() {
		nandFile.readFileLines(mData, SSD_MAX_DATA_SIZE);
		int size = bufferFile.readFileLines(writeBuffer, 10);
		for (int i = 0; i < size; ++i) {
			vector<string> tokens = splitString(writeBuffer[i], ' ');

			if (tokens[0] == "W") {
				mData[stoi(tokens[1])] = tokens[2];
				continue;
			}

			if (tokens[0] == "E") {
				for (int i = 0; i < stoi(tokens[2]); ++i) {
					mData[stoi(tokens[1]) + i] = SSD_DEFAULT_DATA;
				}
				continue;
			}
		}

		nandFile.writeFileLines(mData, SSD_MAX_DATA_SIZE);
		
		writeBuffer[0] = "";
		bufferFile.writeFileLines(writeBuffer, 1);
	}

	void write(int lba, string data) {
		if (isInvalidData(data)) {
			throw invalid_argument("Invalid data");
		}
		if (isInvalidLBA(lba)) {
			throw invalid_argument("Invalid LBA");
		}
  
		int size = bufferFile.readFileLines(writeBuffer, 10);		
		writeBuffer[size++] = "W " + to_string(lba) + " " + data;
		bufferFile.writeFileLines(writeBuffer, size);
		
		if (size >= 10)
			flush();
	}

	void erase(int lba, int size) {
		if (isInvalidLBA(lba)) {
			throw invalid_argument("Invalid LBA");
		}
		if (isInvalidEraseSize(size)) {
			throw invalid_argument("Invalid erase size");
		}

		int len = bufferFile.readFileLines(writeBuffer, 10);
		writeBuffer[len++] = "E " + to_string(lba) + " " + to_string(size);
		bufferFile.writeFileLines(writeBuffer, len);
	
		if (len >= 10)
			flush();
	}
};
