#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "../SSD/Application.cpp"


using namespace std;
using namespace testing;

class MockSSD : public ISSD {
public:
	MOCK_METHOD(void, read, (int), (override));
	MOCK_METHOD(void, write, (int, string), (override));
	MOCK_METHOD(void, erase, (int, int), (override));
	MOCK_METHOD(void, flush, (), (override));
};

class TestSSDFixture : public Test {
private:
	const string TEST_NAND_FILE_PATH = "nand.txt";
	const string TEST_RESULT_FILE_PATH = "result.txt";
	const string TEST_BUFFER_FILE_PATH = "buffer.txt";

	void initNand() {
		ofstream mNandOs(TEST_NAND_FILE_PATH);
		for (int i = 0; i < SSD_MAX_DATA_SIZE; i++) {
			mNandOs << SSD_DEFAULT_DATA << "\n";
		}
		mNandOs.close();
	}
	void initResult() {
		ofstream mResultOs(TEST_RESULT_FILE_PATH);
		mResultOs << SSD_DEFAULT_DATA;
		mResultOs.close();
	}
	void initBuffer() {
		ofstream mResultOs(TEST_BUFFER_FILE_PATH);
		mResultOs.close();
	}
	vector<string> readFile(string filePath) {
		vector<string> data;

		ifstream nandIs(filePath);
		string line;
		while (getline(nandIs, line)) {
			data.push_back(line);
		}
		nandIs.close();

		return data;
	}
	void writeFile(vector<string> data, string filePath) {
		ofstream nandOs(filePath);
		for (string line : data) {
			nandOs << line << "\n";
		}
		nandOs.close();
	}
public:
	void SetUp() override {
		initNand();
		initResult();
		initBuffer();
	}

	void TearDown() override {
		initNand();
		initResult();
		initBuffer();
	}

	void writeNand(int lba, string data) {
		vector<string> nandData = readFile(TEST_NAND_FILE_PATH);
		nandData[lba] = data;
		writeFile(nandData, TEST_NAND_FILE_PATH);
	}

	void writeBufferW(int lba, string data) {
		vector<string> bufferData = readFile(TEST_BUFFER_FILE_PATH);
		string newLine = "W ";
		newLine.append(to_string(lba).append(" "));
		newLine.append(data.append(" "));
		bufferData.push_back(newLine);
		writeFile(bufferData, TEST_BUFFER_FILE_PATH);
	}

	void writeBufferE(int lba, int size) {
		vector<string> bufferData = readFile(TEST_BUFFER_FILE_PATH);
		string newLine = "E ";
		newLine.append(to_string(lba).append(" "));
		newLine.append(to_string(size).append(" "));
		bufferData.push_back(newLine);
		writeFile(bufferData, TEST_BUFFER_FILE_PATH);
	}

	string readNand(int lba) {
		return readFile(TEST_NAND_FILE_PATH)[lba];
	}

	string readResult() {
		return readFile(TEST_RESULT_FILE_PATH)[0];
	}
};

TEST_F(TestSSDFixture, WriteOneData) {
	SSD ssd;

	ssd.write(3, "0x00000001");
	ssd.flush();

	EXPECT_EQ("0x00000001", readNand(3));
}

TEST_F(TestSSDFixture, WriteTwoData) {
	SSD ssd;

	ssd.write(3, "0x00000001");
	ssd.write(5, "0x00000020");
	ssd.flush();

	EXPECT_EQ("0x00000001", readNand(3));
	EXPECT_EQ("0x00000020", readNand(5));
}

TEST_F(TestSSDFixture, WriteThreeData) {
	SSD ssd;

	ssd.write(0, "0x00000001");
	ssd.write(5, "0x00000020");
	ssd.write(99, "0x000ABCDE");
	ssd.flush();

	EXPECT_EQ("0x00000001", readNand(0));
	EXPECT_EQ("0x00000020", readNand(5));
	EXPECT_EQ("0x000ABCDE", readNand(99));
}

TEST_F(TestSSDFixture, WriteInvalidDataLength) {
	SSD ssd;

	EXPECT_THROW(ssd.write(3, "0x01"), invalid_argument);
	EXPECT_THROW(ssd.write(3, "0x0001"), invalid_argument);
	EXPECT_THROW(ssd.write(3, "0x000001"), invalid_argument);
}

TEST_F(TestSSDFixture, WriteInvalidDataFormat) {
	SSD ssd;

	EXPECT_THROW(ssd.write(3, "0000000001"), invalid_argument);
	EXPECT_THROW(ssd.write(3, "xx00000001"), invalid_argument);
	EXPECT_THROW(ssd.write(3, "x000000001"), invalid_argument);
}

TEST_F(TestSSDFixture, WriteInvalidLba) {
	SSD ssd;

	EXPECT_THROW(ssd.write(-1, "0x00000001"), invalid_argument);
	EXPECT_THROW(ssd.write(100, "0x00000001"), invalid_argument);
	EXPECT_THROW(ssd.write(101, "0x00000001"), invalid_argument);
}

TEST_F(TestSSDFixture, ReadEmptySpace) {
	SSD ssd;

	ssd.read(0);

	EXPECT_THAT(readResult(), "0x00000000");
}

TEST_F(TestSSDFixture, ReadZeroLBA) {
	SSD ssd;
	writeNand(0, "0x12345678");

	ssd.read(0);

	EXPECT_THAT(readResult(), "0x12345678");
}

TEST_F(TestSSDFixture, ReadOneLBA) {
	SSD ssd;
	writeNand(0, "0x12345678");
	writeNand(1, "0xABCDEFAB");

	ssd.read(1);

	EXPECT_THAT(readResult(), "0xABCDEFAB");
}

TEST_F(TestSSDFixture, ReadWithInvalidLBA) {
	SSD ssd;

	EXPECT_THROW(ssd.read(-1), out_of_range);
	EXPECT_THROW(ssd.read(100), out_of_range);
}

TEST_F(TestSSDFixture, FastReadErase1) {
	SSD ssd;
	writeNand(2, "0x11223344");
	writeBufferE(2, 5);

	ssd.read(2);

	EXPECT_THAT(readResult(), "0x00000000");
}

TEST_F(TestSSDFixture, FastReadErase2) {
	SSD ssd;
	writeNand(6, "0x11223344");
	writeBufferE(2, 5);

	ssd.read(6);

	EXPECT_THAT(readResult(), "0x00000000");
}

TEST_F(TestSSDFixture, FastReadWrite1) {
	SSD ssd;
	writeNand(5, "0x11223344");
	writeBufferW(5, "0x00001122");

	ssd.read(5);

	EXPECT_THAT(readResult(), "0x00001122");
}

TEST_F(TestSSDFixture, EraseOneData) {
	SSD ssd;
	writeNand(0, "0x00000001");
	writeNand(1, "0x00000020");

	ssd.erase(0, 1);
	ssd.flush();

	EXPECT_EQ("0x00000000", readNand(0));
	EXPECT_EQ("0x00000020", readNand(1));
}

TEST_F(TestSSDFixture, EraseTwoData) {
	SSD ssd;
	writeNand(4, "0x00000020");
	writeNand(5, "0x00000001");
	writeNand(6, "0x00000020");
	writeNand(7, "0x00000300");

	ssd.erase(5, 2);
	ssd.flush();

	EXPECT_EQ("0x00000020", readNand(4));
	EXPECT_EQ("0x00000000", readNand(5));
	EXPECT_EQ("0x00000000", readNand(6));
	EXPECT_EQ("0x00000300", readNand(7));
}

TEST_F(TestSSDFixture, EraseInvalidLba) {
	SSD ssd;

	EXPECT_THROW(ssd.erase(-1, 3), invalid_argument);
	EXPECT_THROW(ssd.erase(100, 3), invalid_argument);
	EXPECT_THROW(ssd.erase(101, 3), invalid_argument);
}

TEST_F(TestSSDFixture, EraseInvalidSize) {
	SSD ssd;

	EXPECT_THROW(ssd.erase(5, -1), invalid_argument);
	EXPECT_THROW(ssd.erase(15, 11), invalid_argument);
	EXPECT_THROW(ssd.erase(20, 12), invalid_argument);
}

TEST(TestSSD, AppInvalidArgument) {
	MockSSD ssd;
	Application app(&ssd);

	EXPECT_CALL(ssd, read(_))
		.Times(0);
	EXPECT_CALL(ssd, write(_, _))
		.Times(0);

	char* cmd[10] = { "SSD.exe", "E", "0","0x00000000" };
	app.run(4, cmd);
}

TEST(TestSSD, AppLessArgument) {
	MockSSD ssd;
	Application app(&ssd);
	app.addCommand(new ReadCommand("Read", "R", 3));
	app.addCommand(new WriteCommand("Write", "W", 4));

	EXPECT_CALL(ssd, read(_))
		.Times(0);
	EXPECT_CALL(ssd, write(_, _))
		.Times(0);

	char* cmd[10] = { "SSD.exe", "R" };
	app.run(2, cmd);
}

TEST(TestSSD, AppArgumentPassing) {
	MockSSD ssd;
	Application app(&ssd);
	app.addCommand(new ReadCommand("Read", "R", 3));
	app.addCommand(new WriteCommand("Write", "W", 4));

	EXPECT_CALL(ssd, read(0))
		.Times(1);

	EXPECT_CALL(ssd, write(0, "0x00000000"))
		.Times(1);

	char* cmd1[10] = { "SSD.exe", "R", "0" };
	app.run(3, cmd1);
	char* cmd2[10] = { "SSD.exe", "W", "0","0x00000000" };
	app.run(4, cmd2);
}

ACTION(ThrowOutOfRangeException) {
	throw out_of_range("LBA is out of range");
}

ACTION(ThrowInvalidArgumentException) {
	throw invalid_argument("Invalid data");
}

TEST(TestSSD, AppExceptionHandle) {
	MockSSD ssd;
	Application app(&ssd);
	app.addCommand(new ReadCommand("Read", "R", 3));
	app.addCommand(new WriteCommand("Write", "W", 4));

	EXPECT_CALL(ssd, read(-1))
		.Times(1)
		.WillOnce(ThrowOutOfRangeException());

	EXPECT_CALL(ssd, write(-1, "0x00000000"))
		.Times(1)
		.WillOnce(ThrowInvalidArgumentException());

	EXPECT_CALL(ssd, write(0, "0x000000"))
		.Times(1)
		.WillOnce(ThrowInvalidArgumentException());

	EXPECT_CALL(ssd, write(0, "0000000000"))
		.Times(1)
		.WillOnce(ThrowInvalidArgumentException());

	char* cmd1[10] = { "SSD.exe", "R", "-1" };
	app.run(3, cmd1);
	char* cmd2[10] = { "SSD.exe", "W", "-1","0x00000000" };
	app.run(4, cmd2);
	char* cmd3[10] = { "SSD.exe", "W", "0","0x000000" };
	app.run(4, cmd3);
	char* cmd4[10] = { "SSD.exe", "W", "0","0000000000" };
	app.run(4, cmd4);
}

TEST(TestSSD, FileReadEmpty) {
	std::ofstream outFile("text.txt");
	outFile << "" << endl;
	outFile.close();

	DataArrayFile file("text.txt");
	string buf[1];
	file.readFileLines(buf, 1);
	EXPECT_THAT(buf[0], Eq(""));

	std::ofstream removeFile("text.txt");
	remove("text.txt");
	removeFile.close();
}

TEST(TestSSD, FileReadTwoLines) {
	std::ofstream outFile("text.txt");
	outFile << "ABCD" << endl;
	outFile << "EFGH" << endl;
	outFile.close();

	DataArrayFile file("text.txt");
	string buf[2];
	file.readFileLines(buf, 2);
	EXPECT_THAT(buf[0], Eq("ABCD"));
	EXPECT_THAT(buf[1], Eq("EFGH"));

	std::ofstream removeFile("text.txt");
	remove("text.txt");
	removeFile.close();
}

TEST(TestSSD, FileWriteTwoLine) {
	DataArrayFile file("text.txt");
	string buf[2] = { "ABCD", "EFGH" };
	file.writeFileLines(buf, 2);

	string line;
	ifstream inFile("text.txt");
	for (int i = 0; i < 2; ++i) {
		getline(inFile, line);
		EXPECT_THAT(line, Eq(buf[i]));
	}
	inFile.close();

	std::ofstream removeFile("text.txt");
	remove("text.txt");
	removeFile.close();
}

TEST(TestSSD, FileReadSize) {
	std::ofstream outFile("text.txt");
	outFile << "ABCD" << endl;
	outFile << "EFGH" << endl;
	outFile << "EFGH" << endl;
	outFile.close();

	DataArrayFile file("text.txt");
	string buf[5];
	int size = file.readFileLines(buf, 2);
	EXPECT_THAT(size, Eq(2));

	std::ofstream removeFile("text.txt");
	remove("text.txt");
	removeFile.close();
}
