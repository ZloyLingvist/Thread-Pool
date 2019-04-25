#pragma once
#pragma warning(disable:4996)
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Tar {
public:
	Tar()=default;
	~Tar()=default;
	virtual int compress(const char *input, const char *output) = 0;
	virtual int decompress(const char *input, const char *output) = 0;
};

