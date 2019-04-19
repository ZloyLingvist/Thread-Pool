#pragma once
#include "Thread_pool.h"
#include "TaskQueue.h"

extern bool v;

Thread_pool::Thread_pool(){ 
	init(); 
}

Thread_pool::Thread_pool(int nThreads){ 
	init(); 
	resize(nThreads); 
}

Thread_pool::~Thread_pool(){
	stop(true);
}

int Thread_pool::size(){
	return static_cast<int>(threads.size()); 
}

int Thread_pool::n_idle(){ 
	return nWaiting; 
}

void Thread_pool::resize(int nThreads){
	if (!isStop && !isDone) {
		int oldNThreads = static_cast<int>(this->threads.size());
		if (oldNThreads <= nThreads){ 
			threads.resize(nThreads);
			flags.resize(nThreads);

			for (int i = oldNThreads; i < nThreads; ++i) {
				flags[i] = std::make_shared<std::atomic<bool>>(false);
				set_thread(i);
			}
		}
		else {  
			for (int i = oldNThreads - 1; i >= nThreads; --i) {
				*flags[i] = true;  
				threads[i]->detach();
			}
			{

				std::unique_lock<std::mutex> lock(mutex);
				cv.notify_all();
			}
			threads.resize(nThreads);  
			flags.resize(nThreads);  
		}
	}
}

void Thread_pool::clear_queue() {
	std::function<void(int id)> * _f;
	while (q.pop(_f)){
		delete _f;
	}
}

std::function<void(int)> Thread_pool::pop(){
	std::function<void(int id)> * _f = nullptr;
	q.pop(_f);
	std::unique_ptr<std::function<void(int id)>> func(_f); 
	std::function<void(int)> f;
	if (_f){
		f = *_f;
	}
	return f;
}

void Thread_pool::stop(bool isWait) {
	isWait = false;
	if (!isWait) {
		if (this->isStop)
			return;
		isStop = true;
		for (int i = 0, n = size(); i < n; ++i) {
			*this->flags[i] = true;  
		}
		clear_queue(); 
	}
	else {
		if (isDone || isStop)
			return;
		isDone = true; 
	}
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.notify_all();  
	}
	for (int i = 0; i < static_cast<int>(threads.size()); ++i){  
		if (threads[i]->joinable()){
			threads[i]->join();
		}
	}
	
	clear_queue();
	threads.clear();
	flags.clear();
}

void Thread_pool::set_thread(int i) {
	std::shared_ptr<std::atomic<bool>> flag(flags[i]); 
	auto f = [this, i, flag]() {
		std::atomic<bool> & _flag = *flag;
		std::function<void(int id)> * _f;
		bool isPop = q.pop(_f);
		while (true) {
			while (isPop) { 
				std::unique_ptr<std::function<void(int id)>> func(_f); 
				(*_f)(i);
				if (_flag) {
					return;
				}
				else {
					isPop = this->q.pop(_f);
				}
			}
			
			std::unique_lock<std::mutex> lock(this->mutex);
			++this->nWaiting;
			this->cv.wait(lock, [this, &_f, &isPop, &_flag]() { isPop = this->q.pop(_f); return isPop || this->isDone || _flag; });
			--this->nWaiting;
			if (!isPop){
				return;
			}
		}
	};
	
	threads[i].reset(new std::thread(f));
}

void Thread_pool::init(){ 
	nWaiting = 0; 
	isStop = false; 
	isDone = false; 
}