#pragma once
#include "Threadpool.h"
#include "TaskQueue.h"

using namespace std;
using namespace my;

Threadpool::Threadpool(unsigned int workers, int maximim_errors, bool verbose) {
	waiting_thread = 0;
	isStop = false;
	isDone = false;
	max_errors = maximim_errors;
	v = verbose;
}

Threadpool::Threadpool(unsigned int workers, TaskQueue &queue,int maximim_errors,bool verbose){
	waiting_thread = 0;
	isStop = false;
	isDone = false;
	max_errors = maximim_errors;
	v=verbose;

	if (!isStop && !isDone) {
		int old_threads = static_cast<int>(threads.size());
		if (old_threads <= workers) {
			threads.resize(workers);
			flags.resize(workers);

			for (int i = old_threads; i < workers; ++i) {
				flags[i] = std::make_shared<atomic<bool>>(false);
				work(queue, i);
			}
		}
		else {
			for (int i = old_threads - 1; i >= workers; --i) {
				*flags[i] = true;  //тред освободился
				threads[i]->detach();
			}
			{
				// останавливаем ждущие треды
				std::unique_lock<std::mutex> lock(mutex);
				data_condition.notify_all();
			}

			threads.resize(workers);
			flags.resize(workers);
		}
	}
}

Threadpool::~Threadpool(){
	stop(true);
}

int Threadpool::size() {
	return static_cast<int>(threads.size());
}


void Threadpool::stop(bool isWait = false) {
	if (!isWait){
		if (isStop){
			return;
		}
		isStop = true;
		for (int i = 0, n = size(); i < n; ++i){
			*flags[i] = true;  //команда треду остановиться
		}
	}
	else {
		if (isDone || isStop){
			return;
		}
		isDone = true;  
	}
	{
		std::unique_lock<std::mutex> lock(mutex);
		data_condition.notify_all();  //остановить все ждущие треды
	}
	for (int i = 0; i < static_cast<int>(threads.size()); ++i){ 
		if (threads[i]->joinable()) {
			threads[i]->join();
		}
	}
	
	threads.clear();
	flags.clear();
}


void Threadpool::work(TaskQueue &queue, int i) {
	int current_errors = max_errors;
	std::shared_ptr<std::atomic<bool>> flag(flags[i]); 

	auto f = [this, i, &current_errors, flag, &queue]() {
		atomic<bool> & flag_stop_thread = *flag;
		bool is_empty = queue.empty();
		while (true) {
			std::unique_lock<std::mutex> lock(this->mutex);
			++waiting_thread;
			data_condition.wait(lock, [this,&is_empty, &flag_stop_thread, &queue]() { 
				is_empty = queue.empty();
				return is_empty || this->isDone || flag_stop_thread;
			});
			--waiting_thread;

			if (is_empty || current_errors==0){ //либо пуста очередь задач, либо превышен лимит ошибок
				return;
			}

			while (!is_empty) { 
				try {
					queue.task_f()();
					queue.pop();
					if (v == true){
						cout.width(10);
						cout << this_thread::get_id() << " Задача " << queue.task_name() << " выполнена " << endl;
					}
				}
				catch (const exception&){
					throw runtime_error("Вызвано исключение у задачи "+queue.task_name());
					--current_errors;
				}

				if (flag_stop_thread) {
					return; 
				}
				else {
					is_empty = queue.empty();
				}
			}
		}
	};

	threads[i].reset(new thread(f)); 
}
