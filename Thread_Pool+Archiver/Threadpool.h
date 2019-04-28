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
* @brief Класс реализующий функциональность тредпула
*/

using namespace std;

class Threadpool {
private:
	std::mutex lock;
	std::condition_variable data_condition;
	std::atomic<bool> active;
	std::vector<std::thread> vector_thread_pool;
	vector<std::function<void(int i)>*> myv;
	int workers = 0; // количество потоков
	bool v = false; // verbose показывать ли 
public:
	/*!
	По количеству потоков в vector_thread_pool закидываются тред. К которым на вход подаются задачи из TaskQueue
	Когда поток освободился он "просит" выдать себе задачу. Когда задачи выполнены, то работа завершается
	\param w -- количество потоков, TaskQueue -- очередь задач
	*/
	Threadpool(int w, TaskQueue &obj, bool verbose);

	/*!
	По количеству потоков в vector_thread_pool закидываются тред. К которым на вход подаются задачи из вектора функций,
	Когда поток освободился он "просит" выдать себе задачу. Когда задачи выполнены, то работа завершается
	\param w -- количество потоков
	*/
	Threadpool(int w, bool verbose);

	/*!
	Пока есть задачи треды их выполняют в цикле. Задачи берутся из TaskQueue
	*/
	void work(TaskQueue &obj);

	/*!
	Проверка есть ли еще задачи в векторе задач TaskQueue. Если есть, то добавляем задачи в очередь
	*/
	bool run(TaskQueue &obj);

	/*!
	Пока есть задачи треды их выполняют в цикле. Задачи берутся из вектора задач тредпула
	*/
	void simple_run();

	/*!
	Добавление функций в вектор задач тредпула
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


