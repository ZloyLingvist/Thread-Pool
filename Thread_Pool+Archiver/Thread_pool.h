#pragma once
#pragma warning(disable:4996)
#include <queue>
#include <future>
#include <mutex>
#include <iostream>
#include <ctime>  
#include <string>

using namespace std;

class Thread_pool{
	private:
		std::mutex lock;
		std::condition_variable data_condition;
		std::atomic<bool> active;
		std::vector<std::thread> vector_thread_pool;
		int workers = 0;
		bool v = false;
	friend class TaskQueue;
	public:
		Thread_pool(int w, TaskQueue &obj, bool verbose);
		Thread_pool(int w, vector<std::function<void()>> myv,bool verbose);

		void work(TaskQueue &obj);
		bool run(TaskQueue &obj,std::thread::id this_id);
		void simple_run(vector<std::function<void()>> myv);
		Thread_pool::~Thread_pool();
};


