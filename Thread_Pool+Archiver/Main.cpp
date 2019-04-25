#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include "libarch.h"
#include "tester.h"
#include <time.h>


int main() {
	setlocale(LC_ALL, "Russian");

	/*------------ Тестирование ------------ */
	cout << "testing" << endl;
	Testing_class obj;
	
	/*cout << "Test functions" << endl;
	cout << "=====Result of T1 is " << obj.test_1(1) << endl;
	cout << "=====Result of T2 is " << obj.test_2(2) << endl;
	cout << "=====Result of T3 is " << obj.test_3("my.tar","in1.txt","in2.txt") << endl;
	cout << "=====Result of T4 is " << obj.test_4(2,2) << endl;
	cout << "=====Result of T5 is " << obj.test_5("in1.txt","in2.txt") << endl;*/

	cout << "Test Archiver+Thread_Pool" << endl;
	cout << "=====Result of T6 is " << obj.test_gl(2,2) << endl;
	
	system("pause");
	return 0;
}
