#pragma once
#pragma warning(disable:4996)
#include<tuple>
#pragma once
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <vector>
#include "tar.h"
#include "archive.h"

using namespace std;

///вспомогательные функции
string creativer();
bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath);
bool exists(const char *name);

bool test_basic(int count);
void work(const char *archname, const char *in, const char *out);