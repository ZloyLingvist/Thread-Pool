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

/**
* @file TaskQueue.h
* @brief Описание очереди задач и структуры задача
*/

namespace my
{
	struct task {
	private:
		unsigned int id_ = 0;
		int priority = 0;
		std::string name{};
		bool doing{ false };
		std::function<void()> func{};
	public:
		~task() = default;
		/*!
		Переопределяем оператор для использования в priority_queue
		*/
		bool operator<(const task &other) const { return priority < other.priority; }

		/*!
		Функция нужная для добавления произвольной функции с произвольным числом параметров в task.
		*/
		template <typename F, typename... Rest>
		auto add_task(std::string &nm, int d, F&& f, Rest&&... rest)->std::future<typename std::result_of<F(Rest...)>::type>
		{
			name = nm;
			priority = d;
			id_ = rand() % 25;
			using namespace std;
			using Return = typename result_of<F(Rest...)>::type;
			using Functor = Return();
			auto pck = make_shared<packaged_task<Functor>>(bind(forward<F>(f), forward<Rest>(rest)...));

			this->func = [pck]() { (*pck)(); };
			return pck->get_future();
		}

		/*!
		Возвращает id текущей задачи (task)
		*/
		int task_id() const
		{
			return this->id_;
		}

		/*!
		\return приоритет текущей задачи (task)
		*/
		int task_priority() const
		{
			return this->priority;
		}

		/*!
		\return имя текущей задачи (task)
		*/
		std::string task_name() const
		{
			return this->name;
		}

		/*!
		\return вернуть функцию текущей задаче (task)
		*/

		std::function<void()> task_func() const
		{
			return this->func;
		}
	};


class TaskQueue
{
private:
	std::size_t sz{};
	int capacity{};
	std::mutex m_mtx{};
	bool v = false;
	std::priority_queue<task> function_queue{};
	friend struct task;
public:
	TaskQueue(int q, bool verbose);

	TaskQueue(const TaskQueue& rhs) = delete;
	TaskQueue& operator=(const TaskQueue& rhs) = delete;

	TaskQueue(TaskQueue&&) = default;
	TaskQueue& operator=(TaskQueue&&) = default;

	/*!
	Проверить очередь на пустоту
	*/
	bool empty() const;

	/*!
	Удалить задачу из TaskQueue
	*/
	bool pop();

	/*!
	\return текущий размер очереди задач
	*/
	std::size_t size_function_queue() const;

	/*!
	Добавить задачу в очередь
	*/
	void push(task t);

	/*!
	\return номер текущей задачи
	*/
	int task_id() const;

	/*!
	\return функцию текущей задачи
	*/
	std::function<void()> task_f() const;

	/*!
	\return имя текущей задачи
	*/
	std::string task_name() const;

	/*!
	\return максимальный размер очереди
	*/
	std::size_t capacity_() const;

	TaskQueue() = default;
};

}

