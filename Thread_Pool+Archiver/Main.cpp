#pragma once
#include <cstdio>
#include <string>
#include <ctime>

#include "Threadpool.h"
#include "TaskQueue.h"
#include "archiver.h"
#include "libarch.h"
#include "tester.h"

using namespace std;

namespace my {
namespace test {

struct example
{
    int a = 0;
    int b = 0;

    void calc()
    {
        cout << "res is " << a + b << endl;
    }
};

inline example calc(/*int i, */int a, int b)
{
    example exam;
    exam.a = 5;
    exam.b = 10;
    exam.calc();
    return exam;
}

inline int examp(/*int k, */int a, int b)
{
    int d = a + b;
    cout << "res is " << d << endl;
    return d;
}

/*void only_thread_pool(int w, bool v)
{
    try
    {
        Threadpool thread_pool(w, v);
        auto future1 = thread_pool.add_function(calc, 5, 4);
        auto future2 = thread_pool.add_function(examp, 10, 2);
        auto future3 = thread_pool.add_function(calc, -23, 14);
        auto future4 = thread_pool.add_function(calc, -32, 4);
        auto future5 = thread_pool.add_function(examp, 1, 2);
    }
    catch (const std::exception&)
    {
        cout << "При тестировании произошла ошибка" << endl;
    }
}*/

void testing_block_1(int log)
{
    try
    {
        Testing_class obj(log);
        cout << "=====Result of T1 is " << obj.test_1(1) << endl;
        cout << "=====Result of T2 is " << obj.test_2(2) << endl;
        cout << "=====Result of T3 is " << obj.test_3("my.tar", "in1.txt", "in2.txt", "2") << endl;//2 или 3 выбор алгоритма сжатия
        cout << "=====Result of T3_2 is " << obj.test_3("my.tar", "in1.txt", "in2.txt", "3") << endl;
        cout << "=====Result of T4 is " << obj.test_2(3) << endl;
        cout << "=====Result of T5 is " << obj.test_5("in1.txt", "in2.txt") << endl;
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

void wro() {
	cout << "///" << endl;
}

int main() {
	setlocale(LC_ALL, "Russian");
	int w = 2;
	int q = 1000;
	bool v = true;
	int log = 0;
	
	//only_thread_pool(w,v);

	/*------------ Тестирование ------------ */

	//testing_block_1(log);
	//testing_block_2(w,q,log);
	testing_block_3(w,q,log);

	
	system("pause");
	return 0;
}
