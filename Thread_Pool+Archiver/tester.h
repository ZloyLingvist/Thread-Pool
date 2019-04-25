#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#include "tar.h"
#include "archive.h"
#include "libarch.h"
#include "own_archiver.h"

using namespace std;

class Testing_class{
public:
	Testing_class()=default;
	~Testing_class()=default;

	vector<string> test_vector;
	double size_after = 0, size_before = 0;
	
	string creativer();
	bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath);
	bool exists(const char *name);
	double filesize(const char* filename);
	void creation_function(int p);
	bool extraction_function(int p);

	bool test_1(int p);
	bool test_2(int p);
	void create_file(int mode,vector<string> &filenames);

	bool sub_test_3();
	template<typename T>
	string toString(T value){
		ostringstream oss;
		oss << value;
		test_vector.push_back(oss.str());
		return oss.str()+" ";
	}

	string merge(std::initializer_list<std::string> strList){
		std::string ret = "";
		for (std::string s : strList){ ret += s;}
		return ret;
	}

	template< typename ... Args >
	bool test_3(const Args& ... args){
		merge({ toString(args)... });
		if (sub_test_3() == true) {
			return true;
		}
		else {
			return false;
		}
	}

	bool test_4(int w,int q);
	bool test_5(const char* in, const char* out);
	bool test_gl(int w, int q);
};

void write(int i, const char *in, int count);