#pragma once
#include <cstdio>
#include <string>
#include <ctime>

#include "Threadpool.h"
#include "TaskQueue.h"
#include "archiver\\archiver.h"
#include "libarch\\libarch.h"
#include "Test\\tester.h"

using namespace std;

namespace my {
namespace test {

inline int examp(int a, int b)
{
    int d = a + b;
    cout << "res is " << d << endl;
    return d;
}

void only_thread_pool(int w, int max_error,bool v)
{
    try
    {
		Threadpool tp(w,max_error, v);
		auto future1=tp.append_function(examp, 1, 2);
    }
    catch (const std::exception&)
    {
        cout << "При тестировании произошла ошибка" << endl;
    }
}

void testing_block_1(int log)
{
    try
    {
        Testing_class obj(log);
        cout << "=====Result of T1 is " << obj.test_1(1) << endl;
        cout << "=====Result of T2 is " << obj.test_2(2) << endl;
        cout << "=====Result of T3 is " << obj.test_3("files\\my.tar", "files\\in1.txt", "files\\in2.txt", "2") << endl;//2 или 3 выбор алгоритма сжатия
        cout << "=====Result of T3_2 is " << obj.test_3("files\\my.tar", "files\\in1.txt", "files\\in2.txt","3") << endl;
        cout << "=====Result of T4 is " << obj.test_2(3) << endl;
        cout << "=====Result of T5 is " << obj.test_5("files\\in1.txt", "files\\in2.txt") << endl;
    }
    catch (const std::exception&)
    {
        cout << "При тестировании произошла ошибка" << endl;
    }
}

void testing_block_2(int w, int q, int log)
{
    try
    {
        Testing_class obj(log);
        cout << "Test work of TaskQueue && ThreadPool" << endl;
        cout << " =====Result of T7 is " << obj.test_7(q) << endl;
        cout << " =====Result of T8 is " << obj.test_8(w) << endl;
        cout << " =====Result of T9 is " << obj.test_9(w, q) << endl;
    }
    catch (const std::exception&)
    {
        cout << "При тестировании произошла ошибка" << endl;
    }
}

void testing_block_3(int w, int q, int log)
{
    try
    {
        Testing_class obj(log);
        cout << " =====Result of T10 is " << obj.test_10(w, q) << endl;
    }
    catch (const std::exception&)
    {
        cout << "При тестировании произошла ошибка" << endl;
    }
}
} // test
} // my

using namespace my;
using namespace my::test;

int add(int a, int b) {
	cout << a+b << endl;
	return a + b;
}

int main() {
	setlocale(LC_ALL, "Russian");
	int w = 2;
	int q = 9;
	bool v = true;
	int log = 1;
	int max_errors = 2;
	
	//only_thread_pool(w,max_errors,v);

	/*------------ Тестирование ------------ */

	//testing_block_1(log);
	//testing_block_2(w,q,log);
	testing_block_3(w,8100,log);

	system("pause");
	return 0;
}
