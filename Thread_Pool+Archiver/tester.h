#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#include "tar.h"
#include "archiver.h"
#include "libarch.h"
#include "ownarchiver.h"
#include "TaskQueue.h"

/**
* @file tester.h
* @brief ����������� �����
*/

using namespace std;

class Testing_class {
private:
	vector<string> test_vector; //����� �������� ����� ������ ��� ������� ����� �������� ������������. ������ �������-��� ������
								//���� ��������� �������� ������� 1, �� ��� ������; 2 (�� ������������ �������� ������). 3 ������������
	double size_after = 0, size_before = 0; // ������ ������ �� � �����

	string creativer(); //���������� ��������� ������. ������������ ��� ���������� ����� � ������������ ������ 
						// ������������ ����������
						//return ��������������� ������ (��� string )

	bool exists(const char *name); //�������� ����� �� �������������
	double filesize(const char* filename); //������ �����
	void creation_function(int p, int log); //������� ���������� � �����. p=1 || 4 ��� ������, p==2 �� ������������ ������ , p==3 �������.
	bool extraction_function(int p, int log);//������� ���������� �� ������. p==1 �������. p==2 ������� � ���������� (�� ����) p==3 ������� � ���������� (����)
	void create_file(int mode, vector<string> &filenames);//������� ����� � ������������ � filenames. 
														  //���� mode=0, �� ������������ ����� ��������� �������������, 
														  //����� ��������������� ����������

	void cleaner();//����� ���������� ������ ������� ����� � ������������ � ������� � ������� test_vector

	bool sub_test_3();//������� test_3 �������� ������������ ����� ���� � �������� ����� �������� ������������. 
					  //sub_test_3 ���������� � ������������� ��� �����. ����� ��� �� ����� ������������ � test_3

					  // ������������ � test_3 ��� ���������� ���������� ������������� �� ��������� ��������
	string merge(std::initializer_list<std::string> strList) {
		std::string ret = "";
		for (std::string s : strList) { ret += s; }
		return ret;
	}

protected:
	int log_test = 0;//������� �� ����� (0) ��� � ����(1) ������� ������������

public:
	Testing_class(int lg) {
		log_test = lg;
	}

	~Testing_class() = default;

	/*!
	�����������. � ����������� �� �������� log_test ���� ������� ������� ������������ ( ��������� msg )
	*/

	void logging(const char *msg);

	/*!
	���������� 2 ����� � ������� lFilePath � rFilePath.
	\return true ���� ����� ������������. false �����
	*/
	bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath);

	/*!
	������� ����� � ��������� ��� �� �������������
	\param p - 1 ������������� ���� � ������������ ������ � ������������ ����������
	\return true ���� �����. false �����
	*/

	bool test_1(int p);

	/*!
	������� �����, ������� ��� ���������� � ���������, ��� ��� ����������� ������ ����� ���� �����������
	\param p p==1 �������. p==2 ������� � ���������� (�� ����) p==3 ������� � ���������� (����)
	\return true ���� �����. false �����
	*/
	bool test_2(int p);

	/*!
	������� �����, ������� ��� ���������� � ���������, ��� ��� ����������� ������ ����� ���� �����������
	\param p p=1 ������������� ���� � ������������ ������ � ������������ ����������
	\return true ���� �����. false �����
	*/
	template<typename T>
	string toString(T value) {
		ostringstream oss;
		oss << value;
		test_vector.push_back(oss.str());
		return oss.str() + " ";
	}

	template< typename ... Args >
	bool test_3(const Args& ... args) {
		merge({ toString(args)... });
		if (sub_test_3() == true) {
			return true;
		}
		else {
			return false;
		}
	}


	bool test_5(const char* in, const char* out);// ���� in ����� � ������� (�������� out) � ������� ����������. 
												 // �������� in � out �� ���������������
	bool test_7(int q);//�������� TaskQueue. �� ���� �������� ����� ������� � ������ ����������.
	bool test_8(int w);//�������� ThreadPool. �� ���� �������� ����� �������
	bool test_9(int w, int q);//�������� TaskQueue+ThreadPool �� ������� �������
	bool test_10(int w, int q);//�������� TaskQueue+ThreadPool+Archiver
};

/*!
\param i,in,count- i-��������� ��������. ���������� in count ���. ������������ � ����� 9
*/
void write(int i, const char *in, int count);

/*!
������������ ��� ������������ �������� �� ������������ ��������
*/
void void_task(int i, int a, int b, int n, const char *text);
/*!
������������ ��� ������������ �������� �� ������������ ��������
*/
int int_task(int i, int a, int b);
/*!
������������ ��� ������������ �������� �� ������������ ��������
*/
string string_test(int i, const char *str1, const char *str2);

/*!
��������� queue ��������; log-����� �� ����� ��� ����
*/
void function_block(TaskQueue &queue, int log);

/*!
�������� ������ name, in1, in2, in3
*/
void removing(const char *name, const char *in1, const char *in2, const char *in3);

/*
������� ���� name
*/
void extract(int i, int log, const char *name);

/*
�� ������������ ������ ������ ... � ���������� �� � ����� name
*/
void compress_own(int i, int log, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2);

/*
������������ ������ ������ ... � ���������� �� � ����� name
*/
void compress_lib(int i, int log, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2);

/*
�� ������������ �������� ������
*/

void decompress_own(int i, const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2);

/*
�� ������������ �������� ������
*/
void decompress_lib(int i, const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2);

/*
�������� ����� file1 � file_res1; file2 � file_res2
*/
void comp(int i, const char *file1, const char *file2, const char *file_res1, const char *file_res2);