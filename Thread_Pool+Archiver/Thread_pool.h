#pragma once
#pragma warning(disable:4996)
#include <queue>
#include <future>
#include <mutex>
#include <iostream>
#include <ctime>  
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <exception>
#include "TaskQueue.h"

using namespace std;

class Thread_pool {
private:
	std::vector<std::unique_ptr<std::thread>> threads;
	std::vector<std::shared_ptr<std::atomic<bool>>> flags;
	TaskQueue<std::function<void(int id)> *> q;
	std::atomic<bool> isDone;
	std::atomic<bool> isStop;
	std::atomic<int> nWaiting;  // how many threads are waiting
	std::mutex mutex;
	std::condition_variable cv;
	void set_thread(int i);
	void init();

public:
	Thread_pool();
	Thread_pool(int nThreads);
	~Thread_pool();
		
	int size();
	int n_idle(); 
	void resize(int nThreads);
	void clear_queue();
	std::function<void(int)> pop();

	void stop(bool isWait);

		template<typename F, typename... Rest>
		auto push(F && f, Rest&&... rest) ->std::future<decltype(f(0, rest...))> {
			auto pck = std::make_shared<std::packaged_task<decltype(f(0, rest...))(int)>>(
				std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
				);
			auto _f = new std::function<void(int id)>([pck](int id) {
				(*pck)(id);
			});
			q.push(_f);
			std::unique_lock<std::mutex> lock(mutex);
			cv.notify_one();
			return pck->get_future();
		}

		template<typename F>
		auto push(F && f) ->std::future<decltype(f(0))> {
			auto pck = std::make_shared<std::packaged_task<decltype(f(0))(int)>>(std::forward<F>(f));
			auto _f = new std::function<void(int id)>([pck](int id) {
				(*pck)(id);
			});
			q.push(_f);
			std::unique_lock<std::mutex> lock(mutex);
			cv.notify_one();
			return pck->get_future();
		}		
};



