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

struct task{
public:
	task() = default;
	~task() = default;

	int id_;
	int priority;
	string name;
	bool doing;
	int workday;
	
	std::function<void(int id)> *func;
	bool operator<(const task &other) const { return priority > other.priority; }

	template<typename F,typename... Rest>
	auto create(F && f,Rest&&... rest) ->std::future<decltype(f(0, rest...))> {
		auto pck = std::make_shared<std::packaged_task<decltype(f(0, rest...))(int)>>(
			std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
			);
		auto _f = new std::function<void(int id)>([pck](int id) {
			(*pck)(id);
		});

		this->id_ = 0;
		this->func = _f;
		this->priority = 1;
		this->workday = 0;
		return pck->get_future();
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
public:
	TaskQueue(int q,bool verbose);
	void add_task(const char *name,int p, int workday,task d);
	bool run(std::thread::id this_id);
	void push_to_end(task d, std::thread::id this_id);
	void print(int mode, std::thread::id this_id); 
	task give_task();
	bool empty();
	void pop();
	TaskQueue() = default;
};



