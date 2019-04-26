#pragma once
#pragma warning(disable:4996)
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

//�������� ������� https://en.wikipedia.org/wiki/Tar_(computing)
//----------- Tar --------------------
//Field offset 	Field size 	Field
//0 	100 	File name
//100 	8 	File mode
//108 	8 	Owner's numeric user ID
//116 	8 	Group's numeric user ID
//124 	12 	File size in bytes(octal base)
//136 	12 	Last modification time in numeric Unix time format(octal)
//148 	8 	Checksum for header record
//156 	1 	Link indicator(file type)
//157 	100 	Name of linked file

//--------USTAR--------
//0 	156 	(several fields, same as in old format)
//156 	1 	Type flag
//157 	100 	(same field as in old format)
//257 	6 	UStar indicator "ustar" then NUL
//263 	2 	UStar version "00"
//265 	32 	Owner user name
//297 	32 	Owner group name
//329 	8 	Device major number
//337 	8 	Device minor number
//345 	155 	Filename prefix

class Tar {
public:
	Tar()=default;
	~Tar()=default;
	virtual int compress(const char *input, const char *output) = 0;
	virtual int decompress(const char *input, const char *output) = 0;
};

