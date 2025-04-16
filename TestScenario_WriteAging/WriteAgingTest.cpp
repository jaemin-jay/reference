#include <string>
#include <iostream>
#include "../TestShell/SsdDriver.cpp"
#include "../TestShell/TCResult.h"

int writeAgingTest() {
	SSD_Driver* ssd = new SSD_Driver("result.txt");

	cout << "\nStep1 > write lba(0~5) value(0xAAAABBBB)" << endl;
	for (int lba = 0; lba <= 5; lba++) {
		for (int i = 0; i < 30; i++) {
			ssd->write(lba, "0xAAAABBBB");
		}
	}

	cout << "\nStep2 > overwrite lba(0~5) value(0x12345678)" << endl;
	for (int lba = 0; lba <= 5; lba++) {
		ssd->write(lba, "0x12345678");
	}

	cout << "\nStep3 > compare lba(0~5) value(0x12345678)" << endl;
	for (int lba = 0; lba <= 5; lba++) {
		string value = ssd->read(lba);
		cout << "Read Value(lba:" << lba << ") :" << value << endl;
		if (value != "0x12345678")
		{
			cout << "\n[DONE] TEST FAILED!" << endl;
			return TCResult::FAIL;
		}
	}
	cout << "\n[DONE] TEST SUCCESS!" << endl;
	return TCResult::PASS;
}

int main()
{
	cout << "\n\n========================" << endl;
	cout << "[Write Aging Test] Start" << endl;
	try {
		exit(writeAgingTest());
	}
	catch (exception& e) {
		cout << e.what() << endl;
		exit(TCResult::ERROR);
	}
}