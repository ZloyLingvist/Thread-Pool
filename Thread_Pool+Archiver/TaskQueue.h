#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <queue>

using namespace std;

/**
* @file TaskQueue.h
* @brief Описание очереди задач и структуры задача
*/

struct task {
private:
	int id_;
	int priority;
	string name;
	bool doing;
	std::function<void(int id)> *func;
public:
	task() = default;
	~task() = default;
	/*!
	Переопределяем оператор для использования в priority_queue
	*/
	bool operator<(const task &other) const { return priority > other.priority; }

	/*!
	Функция нужная для добавления произвольной функции с произвольным числом параметров в task.
	Недостаток в том, что первый аргумент функции должен быть фиктивным и типа int
	*/
	template<typename F, typename... Rest>
	auto add_function(F && f, Rest&&... rest) ->std::future<decltype(f(0, rest...))> {
		auto pck = std::make_shared<std::packaged_task<decltype(f(0, rest...))(int)>>(
			std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
			);
		auto _f = new std::function<void(int id)>([pck](int id) {
			(*pck)(id);
		});

		this->func = _f;
		return pck->get_future();
	}

	/*!
	Возвращает id текущей задачи (task)
	*/
	int task_id() {
		return this->id_;
	}

	/*!
	Добавление названия и приоритета к текущей задаче (task)
	*/

	void make_task(const char *name, int priority) {
		this->name = name;
		this->priority = priority;
	}

	/*!
	\return приоритет текущей задачи (task)
	*/
	int task_priority() {
		return this->priority;
	}

	/*!
	\return имя текущей задачи (task)
	*/
	string task_name() {
		return this->name;
	}

	/*!
	\return вернуть функцию текущей задаче (task)
	*/

	std::function<void(int id)> *task_func() {
		return this->func;
	}
};


class TaskQueue {
private:
	size_t sz;
	int quescap;
	std::mutex m_mtx;
	std::condition_variable m_cond;
	task t;
	bool v = false;
	std::priority_queue<task> function_queue;
	vector<task> task_vector;
	task give_task();
public:
	TaskQueue(int q, bool verbose);
	TaskQueue& operator=(const TaskQueue& rhs) {};

	/*!
	Добавить задачу с именем name и приоритетом p в TaskQueue
	*/
	void add_task(const char *name, int p, task obj);

	/*!
	Добавить задачу в конец TaskQueue
	*/
	void push_to_end(std::thread::id this_id);

	/*!
	Вывести информацию о текущем потоке 1.Задачу запросил 2.Задачу получил 3. Задачу не получил
	*/

	void print(int mode, std::thread::id this_id);

	/*!
	Проверить очередь на пустоту
	*/
	bool empty();

	/*!
	Удалить задачу из TaskQueue
	*/
	void pop();

	/*!
	Проверить вектор задач на пустоту
	*/
	bool check_task_vector();

	/*!
	\return текущий размер очереди задач
	*/
	size_t size_function_queue();

	/*!
	\return максимальный размер очереди задач
	*/
	int return_quescap();

	/*!
	Добавить задачу в очередь
	*/
	void push(int k);

	/*!
	\return номер текущей задачи
	*/
	int task_id();

	/*!
	\return функцию текущей задачи
	*/
	std::function<void(int id)> *task_f();

	/*!
	\return имя текущей задачи
	*/
	string task_name();

	/*!
	Брать задачи из вектора задач. Добавлять их в очередь и выполнять.
	*/
	void simple_run();

	TaskQueue() = default;
};



