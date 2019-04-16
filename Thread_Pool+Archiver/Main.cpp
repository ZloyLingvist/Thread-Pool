#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include <time.h>

bool v = false;
int p = 1;

/*!
Копирует содержимое из исходной области памяти в целевую область память
\param[out] dest Целевая область памяти
\param[in] src Исходная область памяти
\param[in] n Количество байтов, которые необходимо скопировать
*/

void isFilesEqual(const std::string& lFilePath, const std::string& rFilePath){
	std::ifstream lFile(lFilePath.c_str(), std::ifstream::in | std::ifstream::binary);
	std::ifstream rFile(rFilePath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!lFile.is_open() || !rFile.is_open()){
		cout << "Cannot open first or second file" << endl;
	}

	char *lBuffer = new char[1024]();
	char *rBuffer = new char[1024]();

	while (lFile.good() || rFile.good()) {
		lFile.read(lBuffer, 1024);
		rFile.read(rBuffer, 1024);

		if (std::memcmp(lBuffer, rBuffer, 1024) != 0){
			delete[] lBuffer;
			delete[] rBuffer;
			cout << "not equivalent" << endl;
			return;
		}
	}

	delete[] lBuffer;
	delete[] rBuffer;
	cout << "equivalent" << endl;
}


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

	tar.add_to_archive("compress_1.txt","compress_1.txt");
	tar.add_to_archive("compress_1_zlib.txt", "compress_1_zlib.txt");

	tar.close();
}

void extract(){
	Tar tar("archive.tar", 1);
	cout << "Try to extract" << endl;
	tar.extract("archive.tar");
}

void decompress() {
	LZW_archiver obj;
	char *in = "In.txt";
	char *out = "res.txt";

	obj.intf("compress_1.txt",out,2);
	obj.intf("compress_1_zlib.txt","p_zlib.txt",4);

	//cout << "Compare " << in << " with " << out << endl;
	//cout << "Verdict is "; isFilesEqual(in, out);
	cout << "Compare " << in << " with " << "p_zlib.txt" << endl;
	cout << "Verdict is ";
	isFilesEqual(in, "p_zlib.txt");

	//remove("archive.tar");
	remove("compress_1.txt");
	remove("compress_1_zlib.txt");
	remove(out);
}


int main(/*int argc, char* argv[]*/) {
	/* Разбор параметров командной строки */
	int w = 1;
	int q = 1;

	bool v = true;
	int p = 1;
	
	vector<string> arr = { "Создать" ,"Извлечь","Расжать"};
	vector<std::function<void()>> functions = { add,extract,decompress};

	TaskQueue queue(q, arr, functions);
	std::thread::id main_thread_id = std::this_thread::get_id();
	queue.add_task(main_thread_id);

	Thread_pool thread_pool(w);
	thread_pool.init(w, queue);

	return 0;
}
