#include <cstdlib>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

int main() {
	testing::InitGoogleMock();
	RUN_ALL_TESTS();
	system("pause");
}