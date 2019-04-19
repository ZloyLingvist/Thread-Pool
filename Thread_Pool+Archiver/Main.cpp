#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include "libarch.h"
#include "tester.h"
#include <time.h>

bool v = true;
int p = 2;


/*------------ Задачи ---------------*/

void add(int a,const char *in){
	ifstream inFile;
	ofstream outFile;
	lib_archiver obj;
	char *out = "res.txt";

	/* создали архив */
	cout << "Make archive" << endl;
	LZW_archiver tar("archive.tar", 0);
	
	cout << "Try to compress" << endl;
	tar.compress(in, "compress_1.txt");
	obj.compress(in, "compress_2.txt");
	
	tar.add_to_archive("compress_1.txt");
	tar.add_to_archive("compress_2.txt");
	tar.close();
}

void extract(int a,const char *name){
	lib_archiver obj;
	LZW_archiver tar(name, 1);
	cout << "Try to extract" << endl;
	tar.extract(name);
	char *n1 = "compress_1.txt";
	tar.decompress(n1,"res_f.txt");
	obj.decompress("compress_2.txt", "res_s.txt");
}


void write(int a) {
	cout << "I need for number task" << endl;
}


int main() {
	setlocale(LC_ALL, "Russian");

	int w = 2;
	int q = 3;

	bool v = true;
	
	Thread_pool p(2);
	p.push(add,"in3.txt");
	p.push(extract,"archive.tar");
	p.push(write);
	
	/*------------ Тестирование ------------ */
	cout << "testing" << endl;
	Testing_class obj;
	/* Генерирует имена файлов, их количество, содержимое сама программа */
	cout << obj.test_1(1) << endl;
	cout << obj.test_2(2) << endl;
	/* Пользовательский ввод */
	cout << obj.user_test("myarch.tar","in1.txt", "in2.txt",2);
	
	lib_archiver obj2;
	obj2.compress("in1.txt", "t_in1.txt");
	obj2.decompress("t_in1.txt", "TTT.txt");

	system("pause");
	return 0;
}
