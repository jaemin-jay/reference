#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <ctime>
#include <fstream>
using namespace std;

const static string baseFolder = ".\\log\\";
const static string latestFilePath = baseFolder + "latest.log";


#define MOVE_LOG_FILE(FROM, TO) ( string("move ") + (FROM) + string(" ") + (TO) )


class Logger {
public:
	Logger() {
		system((string("mkdir ") + baseFolder).c_str());
		currentLogSize = 0;
	}
	~Logger() {
		logFile.close();
	}


	string print(string log, string func = __builtin_FUNCTION()) {
		openFile();

		string logFormat = collectLog(log, func);
		
		separateLogFiles();
		compressLogFiles();

		return logFormat;
	}

private:
	void openFile(void) {
		if (!logFile.is_open()) {
			logFile.open(latestFilePath.c_str());
			if (!logFile.is_open()) {
				cout << "FILE OPEN FAIL" << endl;
				exit(1);
			}
			currentLogSize = 0;
		}
	}

	void separateLogFiles(void) {
		if (currentLogSize > 10000) {
			logFile.close();
			currentLogSize = 0;

			time_t now = time(0);
			tm* timeinfo = localtime(&now);

			char fileName[size("until_yymmdd_HHh_MMm_SSs.log")];
			strftime(fileName, sizeof(fileName),
				"until_%y%m%d_%Hh_%Mm_%Ss.log", timeinfo);

			string cmd = MOVE_LOG_FILE(latestFilePath, baseFolder + string(fileName));
			system(cmd.c_str());

			logFileList.push(fileName);
		}
	}
	void compressLogFiles(void) {
		while (logFileList.size() > 2) {
			string logfileName = logFileList.front();
			logFileList.pop();

			string cmd = MOVE_LOG_FILE((baseFolder + logfileName), (baseFolder + logfileName + string(".zip")));
			system(cmd.c_str());
		}
	}
	string collectLog(string log, string func) {
		time_t now = time(0);
		tm* timeinfo = localtime(&now);
		char timestamp[size("[yy.mm.dd hh:mm]")];
		strftime(timestamp, sizeof(timestamp),
			"[%y.%m.%d %H:%M]", timeinfo);

		func.append("()");
		while (func.size() < 30) {
			func.append(" ");
		}
		
		string logFormat = string(timestamp) + " " + func + ": " + log + "\n";
		logFile << logFormat;
		currentLogSize += logFormat.size();

		return logFormat;
	}

	ofstream logFile;
	size_t currentLogSize;
	queue<string> logFileList;

};