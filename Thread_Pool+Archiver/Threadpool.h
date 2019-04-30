#pragma once

#include <queue>
#include <future>
#include <mutex>
#include <functional>
#include <type_traits>
#include <iostream>
#include <ctime>  
#include <string>
#include "TaskQueue.h"

/**
* @file Threadpool.h
* @brief Класс реализующий функциональность тредпула
*/

namespace my
{

class Threadpool
{
private:
	int max_errors = 0;
	std::vector<std::unique_ptr<std::thread>> threads;
	std::vector<std::shared_ptr<std::atomic<bool>>> flags;
	std::atomic<bool> isDone;
	std::atomic<bool> isStop;
	std::atomic<int> waiting_thread;  // сколь тредов ждут
	std::mutex mutex;
	std::condition_variable data_condition;
	bool v{ false }; // verbose показывать ли
	int size();
public:
	Threadpool(unsigned int w, TaskQueue &queue, int errors,bool verbose);
	Threadpool(unsigned int workers, int maximim_errors, bool verbose);

	~Threadpool();
	
	void stop(bool isWait);
	void work(TaskQueue &queue, int i);


};

}
