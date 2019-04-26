#pragma once
#include "Thread_pool.h"
#include "TaskQueue.h"

extern bool v;

Thread_pool::Thread_pool(int w, TaskQueue &obj, bool verbose) : lock(), data_condition(), active(true), vector_thread_pool(){
	workers = w; 
	for (int i = 0; i < workers; i++) {
		vector_thread_pool.emplace_back(std::thread(&Thread_pool::work, this, std::ref(obj)));
	}
	
	run(obj);
	v = verbose;
}

Thread_pool::Thread_pool(int w,bool verbose) : lock(), data_condition(), active(true), vector_thread_pool(){
	workers = w;
	for (int i = 0; i < workers; i++) {
		vector_thread_pool.emplace_back(std::thread(&Thread_pool::simple_run, this));
	}
}

Thread_pool::~Thread_pool() {
	for (auto &t : vector_thread_pool) {
		t.join();
	}
}

void Thread_pool::work(TaskQueue &obj){
	bool v = true;
	std::function<void(int id)> *func;
	task d;
	int id = 0;
	int error = 0;
	string name = "";
	string error_name="";
	std::thread::id this_id = std::this_thread::get_id();
	
	while (true) {
		{
			std::unique_lock<std::mutex> lock_(lock);
			data_condition.wait(lock_,[this_id,this,&obj](){
				cout << endl;
				return !obj.empty(this_id) || !active;
			});

			if (!active && obj.empty(this_id)) {
				obj.print(1, this_id);
				if (run(obj)==false){
					cout << "Задач для " << this_id << " нет" << endl;
					return;
				}

				error = 0;
				obj.print(4, this_id);
			}

			name = obj.task_name();
			if (error < 2 && error_name != name){
				func = obj.task_f();
				id = obj.task_id();
				obj.pop();
			}
			else {
				obj.pop();
				name = obj.task_name();
				func = obj.task_f();
				id = obj.task_id();
				error = 0;
			}

			active = false;
			data_condition.notify_all();
		}

		try {
			(*func)(id);
			if (v == true) {
				cout.width(10);
				std::cout << this_id << " Задача " << name << " выполнена " << std::endl;
			}
		}

		catch (const std::exception &e) {
			if (v == true){
				cout.width(10);
				std::cout << this_id << " Вызвано исключение у задачи " << id << std::endl;
			}
			
			obj.push_to_end(this_id);
			error_name = name;
			error = error + 1;
		}	
	}
}

bool Thread_pool::run(TaskQueue &obj) {
	int k = 0;
	if (obj.check_task_vector() == true) {
		return false;
	}

	for (int i = 0; i < (1 + rand() % obj.return_quescap()); i++) {
		obj.push(k);
		k = k + 1;
	}

	return true;
}

void Thread_pool::simple_run() {
	bool v = true;
	std::function<void(int id)> *func;
	task d;
	int id = 0;
	int error = 0;
	string name = "";
	string error_name = "";
	std::thread::id this_id = std::this_thread::get_id();

	while (true) {
		{
			std::unique_lock<std::mutex> lock_(lock);
			data_condition.wait(lock_, [this_id, this]() {
				cout << endl;
				return !myv.empty() || !active;
			});

			if (!active && myv.empty()) {
				cout << "Задач для " << this_id << " нет" << endl;
				return;
			}

			func = myv.back();
			id = myv.size();
			if (myv.empty() == false){
				myv.pop_back();
			}
			active = false;
			data_condition.notify_all();
		}

		try {
			(*func)(id);
			cout.width(10);
			std::cout << this_id << " Задача " << name << " выполнена " << std::endl;
		}

		catch (const std::exception &e) {
			cout.width(10);
			std::cout << this_id << " Вызвано исключение у задачи " << id << std::endl;
		}
	}
}

