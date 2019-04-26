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
#include "TaskQueue.h"


using namespace std;

class Testing_class{
public:
	Testing_class()=default;
	~Testing_class()=default;

	vector<string> test_vector;
	double size_after = 0, size_before = 0;
	
	///вспомогательные функции
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

	
	bool test_5(const char* in, const char* out);
	bool test_7(int q);
	bool test_8(int w);
	bool test_9(int w, int q);
	bool test_10(int w, int q);
};

void write(int i, const char *in, int count);

void void_task(int i, int a, int b, int n, const char *text);
int int_task(int i, int a, int b);
string string_test(int i, const char *str1, const char *str2);

void function_block(TaskQueue &queue);
void removing(const char *name, const char *in1, const char *in2, const char *in3);
void extract(int i, const char *name);
void comp(int i, const char *file1, const char *file2, const char *file_res1, const char *file_res2);
void function_block(TaskQueue &queue);
void removing(const char *name, const char *in1, const char *in2, const char *in3);
void extract(int i, const char *name);
void compress_own(int i, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2);
void compress_lib(int i, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2);
void decompress_own(int i, const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2);
void decompress_lib(int i, const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2);
void comp(int i, const char *file1, const char *file2, const char *file_res1, const char *file_res2);