#pragma once
#include "Thread_pool.h"
#include "TaskQueue.h"

extern bool v;

Thread_pool::Thread_pool(int w, TaskQueue &obj, bool verbose) : lock(), data_condition(), active(true), vector_thread_pool(){
	workers = w; 
	for (int i = 0; i < workers; i++) {
		vector_thread_pool.emplace_back(std::thread(&Thread_pool::work, this, std::ref(obj)));
	}
	v = verbose;
}
Thread_pool::~Thread_pool() {
	for (auto &t : vector_thread_pool) {
		t.join();
	}
}

void Thread_pool::work(TaskQueue &obj){
	bool v = true;
	std::function<void(int id)> func;
	task d;
	int id = 0;
	int error = 0;
	int workday = 0;
	string name = "";
	string error_name="";
	std::thread::id this_id = std::this_thread::get_id();
	
	while (true) {
		{
			std::unique_lock<std::mutex> lock_(lock);
			data_condition.wait(lock_,[this,&obj](){
				return !obj.empty() || !active;
			});

			if (!active && obj.empty()) {
				obj.print(1, this_id);
				bool v1=obj.run(this_id);
				if (v1 == false){
					cout << "Задач нет" << endl;
					return;
				}
				error = 0;
				obj.print(4, this_id);
			}

			d = obj.give_task();
			name = d.name;
			workday = d.workday;

			if (error < 2 && error_name != name) {
				func = *d.func;
				id = d.id_;
				workday = d.workday;
				obj.pop();
			}
			else {
				obj.pop();
				cout.width(10);
				obj.run(this_id);
				d = obj.give_task();
				name = d.name;
				func = *d.func;
				id = d.id_;
				workday = d.workday;
				error = 0;
			}

			active = false;
			data_condition.notify_all();
			srand(time(NULL));
			if (v == true) {
				cout.width(10);
				cout << this_id << " Отправляем в сон " << endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(workday));
		}

		try {
			func(id);
			if (v == true) {
				cout.width(10);
				std::cout << this_id << " Задача " << d.name << " c приоритетом " << d.priority << " выполнена " << std::endl;
			}
		}

		catch (const std::exception &e) {
			if (v == true){
				cout.width(10);
				std::cout << this_id << " Вызвано исключение у задачи " << d.id_ << " c приоритетом " << d.priority << std::endl;
			}
			
			obj.push_to_end(d,this_id);
			error_name = name;
			error = error + 1;
		}	
	}
}
	