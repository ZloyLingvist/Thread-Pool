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
private:
	int id_;
	int priority;
	string name;
	bool doing;
	std::function<void(int id)> *func;
public:
	task() = default;
	~task() = default;
	bool operator<(const task &other) const { return priority > other.priority; }

	template<typename F,typename... Rest>
	auto add_function(F && f,Rest&&... rest) ->std::future<decltype(f(0, rest...))> {
		auto pck = std::make_shared<std::packaged_task<decltype(f(0, rest...))(int)>>(
			std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
			);
		auto _f = new std::function<void(int id)>([pck](int id) {
			(*pck)(id);
		});

		this->func = _f;
		return pck->get_future();
	}

	int task_id(){
		return this->id_;
	}

	void make_task(const char *name,int priority){
		this->name = name;
		this->priority = priority;
	}

	int task_priority() {
		return this->priority;
	}

	string task_name() {
		return this->name;
	}

	std::function<void(int id)> *task_func(){
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
	TaskQueue(int q,bool verbose);
	TaskQueue& operator=(const TaskQueue& rhs) {};

	void add_task(const char *name,int p,task obj);
	void push_to_end(std::thread::id this_id);
	void print(int mode, std::thread::id this_id); 
	bool empty(std::thread::id this_id);
	void pop();

	bool check_task_vector();
	size_t size_function_queue();
	int return_quescap();
	void push(int k);
	int task_id();
	std::function<void(int id)> *task_f();
	string task_name();

	void simple_run();
	
	TaskQueue() = default;
};



