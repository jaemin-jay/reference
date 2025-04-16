#pragma once
#include<string>
#include<iostream>
#include <fstream>
#include <vector>
#include "TCResult.h"
using namespace std;

class TestCase {
public:
    TestCase(string _cmd, string _name) : cmd(_cmd), name(_name) {

    }

    string getCmd() {
        return cmd;
    }

    string getName() {
        return name;
    }

private:
    string cmd;
    string name;
};

class TCManager {
public:
    static const int STDOUT_REDIRECTION_ON = 1;
    static const int STDOUT_REDIRECTION_OFF = 2;

    TCManager(string filename, int _redirection) : redirection(_redirection) {
        vector<string> tclist = readLinesFromFile(filename);
        int tcid = 1;
        for (string tcname : tclist) {
            string cmd = "testapp" + to_string(tcid);
            testcases.push_back(TestCase{ cmd, tcname });
            tcid++;
        }
	}

    vector<string> readLinesFromFile(const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {    
            file << "TestScenario_FullWrite" << std::endl;
            file << "TestScenario_WriteAging" << std::endl;
            file.close();
        }
        else {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
        }

        ifstream infile(filename);
        vector<string> lines;
        if (infile) {
            std::string line;
            while (std::getline(infile, line)) {
                lines.push_back(line);
            }

            infile.close();
        }
        else {
            cerr << "[TCManager][ERROR] file open failed." << endl;
        }

        return lines;
    }

    int run(string cmd) {
        for (TestCase tc : testcases) {
            if (cmd == tc.getCmd()) {
                string cmd = tc.getName();
                if (redirection == TCManager::STDOUT_REDIRECTION_ON)
                     cmd += ".exe >> test_result.log";
                else
                     cmd += ".exe";

                return system(cmd.c_str());
            }
        }
        return TCResult::FAIL;
    }

    void printTestCases() {
        cout << "\n[ TestCases ]" << endl;
        for (TestCase tc : testcases) {
            cout << "- " << tc.getCmd() << " : " << tc.getName() << endl;
        }
    }



private:
    vector<TestCase> testcases;
    int redirection = STDOUT_REDIRECTION_ON;
};