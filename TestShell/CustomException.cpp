#pragma once
#include <iostream>
#include <exception>

using namespace std;

class InvalidInputException : public exception {
public:
	explicit InvalidInputException(char const* message) : exception(message) {

	}
};

class ExitException : public exception {
public:
	explicit ExitException(char const* _msg) :exception(_msg) {

	}
};