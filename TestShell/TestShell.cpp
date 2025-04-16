#pragma once
#include <string>
#include <vector>
#include <iostream>

#include "ISSD.h"
#include "CustomException.cpp"
#include "TCManager.cpp"

using namespace std;

class TestShell {
public:
	explicit TestShell(ISSD* _ssd) : ssd(_ssd) {
		tcManager = new TCManager("tclist", TCManager::STDOUT_REDIRECTION_OFF);
	}

	void start() {
		while (1) {
			try {
				processCommand(getCommand());
			}
			catch (InvalidInputException e) {
				cout << e.what() << endl;
				while (std::cin.get() != '\n');
				help();
			}
			catch (ExitException e) {
				cout << e.what() << endl;
				break;
			}
		}
	}

	string getCommand() {
		string command;
		cout << "\nCMD > ";
		cin >> command;
		return command;
	}
	
	void processCommand(string command) {
		if (command == "fullread") {
			fullread();
		}
		else if (command == "fullwrite") {
			getValue();
			fullwrite(this->value);
		}
		else if (command == "read") {
			getLba();
			read(this->lba);
		}
		else if (command == "write") {
			getLba();
			getValue();
			write(this->lba, this->value);
		}
		else if (command == "exit") {
			terminateProcess();
		}
		else if (command == "help") {
			help();
		}
		else {
			if (tcManager->run(command) < 0)
				throw InvalidInputException("Invalid Command");
		}
	}

	bool isNumber(const std::string& str) {
		for (char c : str) {
			if (!std::isdigit(c)) {
				return false;
			}
		}
		return true;
	}

	void getLba() {
		string str_lba;
		cin >> str_lba;

		if (!isNumber(str_lba))
			throw InvalidInputException("Invalid Lba");

		this->lba = stoi(str_lba);
		if (0 > this->lba || this->lba > 99)
			throw InvalidInputException("Invalid Lba");
	}

	void getValue() {
		cin >> this->value;
		if (this->value.size() != 10 || this->value.substr(0, 2) != "0x")
			throw InvalidInputException("Invalid Value");

		for (char ch_value : this->value.substr(2, 10)) {
			if ('0' <= ch_value && ch_value <= '9') continue;
			if ('A' <= ch_value && ch_value <= 'F') continue;
			throw InvalidInputException("Invalid Value");
		}
	}

	void terminateProcess() {
		throw ExitException("Exit Program");
	}

	void write(int lba, string value) {
		ssd->write(lba, value);
	}

	string read(int lba) {
		string value = ssd->read(lba);
		cout << value << endl;

		return value;
	}

	void fullread() {
		for (int lba = 0; lba < 100; lba++) {
			read(lba);
		}
	}

	void fullwrite(string value) {
		for (int lba = 0; lba < 100; lba++) {
			write(lba, value);
		}
	}

	void help() {
		cout
			<< "[ CMD ]\n"
			<< "- write {no} {data} : {data} was recorded in LBA {no}\n"
			<< "-- {data} : hexadecimal \n"
			<< "-- ex. write 3 0xAAAABBBB\n"
			<< "- read {no} : Read LBA {no} times\n"
			<< "- fullwrite {value} : 0~99 LBA Write\n"
			<< "- fullread : 0~99 LBA Read\n"
			<< "- exit : shell exits\n"
			<< "- help : Displays how to use each command\n";
		tcManager->printTestCases();
	}

private:
	ISSD* ssd;
	TCManager* tcManager;
	int lba;
	string value;
}
;