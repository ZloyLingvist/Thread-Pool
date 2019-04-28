#pragma warning(disable:4996)
#include <queue>
#include <future>
#include <mutex>
#include <iostream>
#include <ctime>  
#include <string>
#include "TaskQueue.h"

/**
* @file Threadpool.h
* @brief ����� ����������� ���������������� ��������
*/

using namespace std;

class Threadpool {
private:
	std::mutex lock;
	std::condition_variable data_condition;
	std::atomic<bool> active;
	std::vector<std::thread> vector_thread_pool;
	vector<std::function<void(int i)>*> myv;
	int workers = 0; // ���������� �������
	bool v = false; // verbose ���������� �� 
public:
	/*!
	�� ���������� ������� � vector_thread_pool ������������ ����. � ������� �� ���� �������� ������ �� TaskQueue
	����� ����� ����������� �� "������" ������ ���� ������. ����� ������ ���������, �� ������ �����������
	\param w -- ���������� �������, TaskQueue -- ������� �����
	*/
	Threadpool(int w, TaskQueue &obj, bool verbose);

	/*!
	�� ���������� ������� � vector_thread_pool ������������ ����. � ������� �� ���� �������� ������ �� ������� �������,
	����� ����� ����������� �� "������" ������ ���� ������. ����� ������ ���������, �� ������ �����������
	\param w -- ���������� �������
	*/
	Threadpool(int w, bool verbose);

	/*!
	���� ���� ������ ����� �� ��������� � �����. ������ ������� �� TaskQueue
	*/
	void work(TaskQueue &obj);

	/*!
	�������� ���� �� ��� ������ � ������� ����� TaskQueue. ���� ����, �� ��������� ������ � �������
	*/
	bool run(TaskQueue &obj);

	/*!
	���� ���� ������ ����� �� ��������� � �����. ������ ������� �� ������� ����� ��������
	*/
	void simple_run();

	/*!
	���������� ������� � ������ ����� ��������
	*/
	template<typename F, typename... Rest>
	auto add_function(F && f, Rest&&... rest) ->std::future<decltype(f(0, rest...))> {
		auto pck = std::make_shared<std::packaged_task<decltype(f(0, rest...))(int)>>(
			std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
			);
		auto _f = new std::function<void(int id)>([pck](int id) {
			(*pck)(id);
		});

		myv.push_back(_f);
		return pck->get_future();
	}

	Threadpool::~Threadpool();
};


