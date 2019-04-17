#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <future>
#include <mutex>
#include <iostream>
#include <ctime>  
#include <string>

using namespace std;

extern bool v;
extern int p;

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
	task t;
	int r = 0;
	int k = 0;
	vector<string> myvec;
	vector<std::function<void()>> functions;
	TaskQueue(int q, vector<string> vec, vector<std::function<void()>> func);

	int now(){
		return k;
	 }

	void add_task(std::thread::id this_id);
	void push_to_end(task d, std::thread::id this_id); 
	void print(int mode, std::thread::id this_id);
	task give_task();
	bool empty();
	void pop();
	TaskQueue::~TaskQueue() {}
};


