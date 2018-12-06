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
	friend class TaskQueue;
	public:
		Thread_pool(int w);
		void init(int workers, TaskQueue &obj);
		void work(TaskQueue &obj);
		Thread_pool::~Thread_pool();
};


