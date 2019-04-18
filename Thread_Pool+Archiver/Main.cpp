#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include "tester.h"
#include <time.h>

bool v = true;
int p = 2;


/*------------ Задачи ---------------*/

void add(){
	ifstream inFile;
	ofstream outFile;
	LZW_archiver obj;

	char *in = "in1.txt";
	char *out = "res.txt";

	/* создали архив */
	cout << "Make archive" << endl;
	Tar tar("archive.tar", 0);

	cout << "Try to compress" << endl;
	obj.intf(in, "compress_1.txt", 1); //1-сжатие 2-расжатие - внутреннее 3,4 библиотечное
	
	tar.add_to_archive("compress_1.txt");
	tar.close();
}

void extract(){
	char *name = "archive.tar";
	Tar tar(name, 1);
	cout << "Try to extract" << endl;
	tar.extract(name);
}

void decompress() {
	LZW_archiver obj;
	char *in = "In.txt";
	char *out = "res.txt";

	char *n1 = "compress_1.txt";
	obj.intf(n1,out,2);
}

void write() {
	cout << "I need for number task" << endl;
}

int main(/*int argc, char* argv[]*/) {
	setlocale(LC_ALL, "Russian");

	int w = 2;
	int q = 3;

	bool v = true;
	int p = 1;
	
	vector<string> arr = { "Создать" ,"Извлечь","Расжать","Написать"};
	vector<std::function<void()>> functions = { add,extract,decompress,write};

	TaskQueue queue(q, arr, functions);
	std::thread::id main_thread_id = std::this_thread::get_id();
	queue.add_task(main_thread_id);

	Thread_pool thread_pool(w);
	thread_pool.init(w, queue);

	
	/*------------ Тестирование ------------ */
	//Testing_class obj;
	/* Генерирует имена файлов, их количество, содержимое сама программа */
	//cout << obj.test_1(1) << endl;
	//cout << obj.test_2(2) << endl;
	/* Пользовательский ввод */
	//cout << obj.user_test("myarch.tar","in1.txt", "in2.txt",2);
	//cout << endl;
	//system("pause");
	return 0;
}
