#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include "tester.h"
#include <time.h>

bool v = false;
int p = 1;


/*------------ Задачи ---------------*/

void add(){
	ifstream inFile;
	ofstream outFile;
	LZW_archiver obj;

	char *in = "In.txt";
	char *out = "res.txt";

	/* создали архив */
	cout << "Make archive" << endl;
	Tar tar("archive.tar", 0);

	cout << "Try to compress" << endl;
	obj.intf(in, "compress_1.txt", 1); //1-сжатие 2-расжатие - внутреннее 3,4 библиотечное
	obj.intf(in, "compress_1_zlib.txt", 3); 

	tar.add_to_archive("compress_1.txt");
	tar.add_to_archive("compress_1_zlib.txt");

	tar.close();
}

void extract(){
	char *name = "archiver.tar";
	int res = 0;
	/*if (FILE *file = fopen(name, "r")) {
		fclose(file);
	}
	else {
		throw("file is not consist");
	}*/

	Tar tar("NSV5gRnTIDKc.tar", 1);
	cout << "Try to extract" << endl;
	tar.extract("NSV5gRnTIDKc.tar");
}

void decompress() {
	LZW_archiver obj;
	char *in = "In.txt";
	char *out = "res.txt";

	char *n1 = "O3t.txt";
	char *n2 = "compress_1_zlib.txt";
	
	/*if (FILE *file = fopen(n1, "r")) {
		fclose(file);
	}
	else {
		throw("file is not consist");
	}

	if (FILE *file = fopen(n2, "r")) {
		fclose(file);
	}
	else {
		throw("file is not consist");
	}*/

	obj.intf(n1,out,4);
	//obj.intf(n2,"p_zlib.txt",4);

	//cout << "Compare " << in << " with " << out << endl;
	//cout << "Verdict is "; isFilesEqual(in, out);
	//cout << "Compare " << in << " with " << "p_zlib.txt" << endl;
	//cout << "Verdict is ";
	//isFilesEqual(in, "p_zlib.txt");

	//remove("archive.tar");
	//remove("compress_1.txt");
	//remove("compress_1_zlib.txt");
	//remove(out);
}


int main(/*int argc, char* argv[]*/) {
	/* Разбор параметров командной строки */
	int w = 1;
	int q = 2;

	bool v = true;
	int p = 1;
	
	/*vector<string> arr = { "Создать" ,"Извлечь","Расжать"};
	vector<std::function<void()>> functions = { add,extract,decompress,write};

	TaskQueue queue(q, arr, functions);
	std::thread::id main_thread_id = std::this_thread::get_id();
	queue.add_task(main_thread_id);

	Thread_pool thread_pool(w);
	thread_pool.init(w, queue);*/

	cout << test_basic(2) << endl;
	//work("myarch.tar", "24.txt", "res.txt");
	
	system("pause");

	return 0;
}
