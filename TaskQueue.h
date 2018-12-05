#pragma once
#include <fstream>
#include <iostream>

using namespace std;

extern bool v;

struct task {
	int id;
	int priority;
	string name;
	bool doing;
	std::function<void()> f;
	bool operator<(const task &other) const { return priority < other.priority; }
};

class TaskQueue {
private:
	std::priority_queue<task> function_queue;
	size_t sz;
	int quescap;
	std::mutex m_mtx;
	std::condition_variable m_cond;
public:
	TaskQueue(int q) { quescap = q; }

	void push(task t) {
		sz = function_queue.size();
		if ((sz + 1) <= quescap) {
			std::unique_lock<std::mutex> lock_(m_mtx);
			function_queue.push(t);
			lock_.unlock();
			m_cond.notify_one();
		}
		else {
			cout << "Не могу добавить в очередь" << endl;
		}
	}

	void sort() {
		function_queue.top();
	}

	void print() {
		task d = function_queue.top();
		std::cout << d.name << endl;
		std::cout << d.priority << endl;
	}

	task give_task(){
		task d=function_queue.top();
		return d;
	}

	bool empty(){
		bool val=function_queue.empty();
		return val;
	}

	void pop(){
		std::unique_lock<std::mutex> lock_(m_mtx);
		m_cond.wait(lock_, [this]() {return !function_queue.empty();
		});
		
		function_queue.pop();
	}

	TaskQueue::~TaskQueue() {}
};


