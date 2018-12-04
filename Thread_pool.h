#pragma once
#pragma warning(disable:4996)
#include <queue>
#include <future>
#include <mutex>
#include <iostream>
#include <ctime>  
#include <string>

using namespace std;

struct task {
	int id;
	int priority;
	string name;
	bool doing;
	std::function<void()> f;
	bool operator<(const task &other) const { return priority < other.priority; }
};

class Thread_pool{
	private:
		std::priority_queue<task> function_queue;
		std::mutex lock;
		std::condition_variable data_condition;
		std::atomic<bool> accept_functions;
	public:
		Thread_pool();
		~Thread_pool();
		void push(int id,std::function<void()> func,int priority,bool doing,string name);
		void finish();
		void work();
};