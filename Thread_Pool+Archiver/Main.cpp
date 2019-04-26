#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include "libarch.h"
#include "tester.h"
#include <time.h>

struct example {
	int a = 0;
	int b = 0;
	
	void calc(){
		cout << "res is " << a + b << endl;
	}
};

example calc(int i, int a, int b) {
	example exam;
	exam.a = 5;
	exam.b = 10;
	exam.calc();
	return exam;
}

int examp(int k, int a, int b) {
	int d = a + b;
	cout << "res is " << d << endl;
	return d;
}

void only_thread_pool(int w,bool v) {
	Thread_pool thread_pool(w, v);
	thread_pool.add_function(calc, 5, 4);
	thread_pool.add_function(examp, 10, 2);
}

void testing_block_1() {
	Testing_class obj;
	cout << "=====Result of T1 is " << obj.test_1(1) << endl;
	cout << "=====Result of T2 is " << obj.test_2(2) << endl;
	cout << "=====Result of T3 is " << obj.test_3("my.tar", "in1.txt", "in2.txt","2") << endl;//2 или 3 выбор алгоритма сжатия
	cout << "=====Result of T3_2 is " << obj.test_3("my.tar", "in1.txt", "in2.txt", "3") << endl;
	cout << "=====Result of T4 is " << obj.test_2(3) << endl;
	cout << "=====Result of T5 is " << obj.test_5("in1.txt","in2.txt") << endl;
}

void testing_block_2(int w,int q) {
	Testing_class obj;
	cout << "Test work of TaskQueue && ThreadPool" << endl;
	cout << " =====Result of T7 is " << obj.test_7(q) << endl;
	cout << " =====Result of T8 is " << obj.test_8(w) << endl;
	cout << " =====Result of T9 is " << obj.test_9(w,q) << endl;
}

void testing_block_3(int w,int q) {
	Testing_class obj;
	cout << " =====Result of T10 is " << obj.test_10(w,q) << endl;
}

int main() {
	setlocale(LC_ALL, "Russian");
	int w = 2;
	int q = 3;
	bool v = true;
	
	//only_thread_pool(w,v);

	/*------------ Тестирование ------------ */
	//testing_block_1();
	//testing_block_2(2, 2);
	testing_block_3(w, q);
	
	system("pause");
	return 0;
}
