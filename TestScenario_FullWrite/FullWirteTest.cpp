#include <string>
#include <iostream>
#include "../TestShell/SsdDriver.cpp"
#include "../TestShell/TCResult.h"

using namespace std;

int fullWriteTest() {
	SSD_Driver* ssd = new SSD_Driver("result.txt");

	string writeValue = "0x01234567";
	
	for (int lba = 0; lba < 100; lba++)
		ssd->write(lba, writeValue);

	for (int lba = 0; lba < 100; ++lba) {
		string value = ssd->read(lba).substr(0, 10);
		if (value != writeValue) {
			cout << "FullWrite Test Failed - LBA : " << lba << ", value : " << value << endl;
			return TCResult::FAIL;
		}
	}

	cout << "[DONE] FullWrite Test Success" << endl;
	return TCResult::PASS;
}

int main() {
	cout << "\n\n========================" << endl;
	cout << "[FullWrite Test] Start" << endl;

	try {
		exit(fullWriteTest());
	}
	catch (exception& e) {
		cout << e.what() << endl;
		exit(TCResult::ERROR);
	}
}