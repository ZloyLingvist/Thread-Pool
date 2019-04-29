#pragma once
#include "Threadpool.h"
#include "TaskQueue.h"

using namespace std;
using namespace my;

extern bool v;

Threadpool::Threadpool(int w, TaskQueue &obj, bool verbose) : lock(), data_condition(), active(true), vector_thread_pool(){
	workers = w; 
	for (int i = 0; i < workers; i++) {
		vector_thread_pool.emplace_back(thread(&Threadpool::work, this, ref(obj)));
	}
	
	run(obj);
	v = verbose;
}

Threadpool::Threadpool(int w,bool verbose) : lock(), data_condition(), active(true), vector_thread_pool(){
	workers = w;
	for (int i = 0; i < workers; i++) {
		vector_thread_pool.emplace_back(thread(&Threadpool::simple_run, this));
	}
	v = verbose;
}

Threadpool::~Threadpool() {
	for (auto &t : vector_thread_pool) {
		t.join();
	}
}

void Threadpool::work(TaskQueue &obj){
	bool v = true;
	function<void()> func;
	task d;
	int id = 0;
	int error = 0;
	string name = "";
	string error_name="";
	thread::id this_id = this_thread::get_id();
	
	while (true) {
		{
			unique_lock<mutex> lock_(lock);
			data_condition.wait(lock_,[this,&obj](){
				cout << endl;
				return !obj.empty() || !active;
			});

			if (!active && obj.empty()) {
				obj.print(1, this_id);
				if (run(obj)==false){
					if (v == true) {
						cout << "Задач для " << this_id << " нет" << endl;
					}
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
			func();
			if (v == true) {
				cout.width(10);
				cout << this_id << " Задача " << name << " выполнена " << endl;
			}
		}

		catch (const exception&) {
			if (v == true){
				cout.width(10);
				cout << this_id << " Вызвано исключение у задачи " << id << endl;
			}
			
			obj.push_to_end(this_id);
			error_name = name;
			error = error + 1;
		}	
	}
}

bool Threadpool::run(TaskQueue &obj) {
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

void Threadpool::simple_run() {
	bool v = true;
	function<void()> func;
	task d;
    size_t id = 0;
	int error = 0;
	string name = "";
	string error_name = "";
	thread::id this_id = this_thread::get_id();

	while (true) {
		{
			unique_lock<mutex> lock_(lock);
			data_condition.wait(lock_, [this_id, this]() {
				cout << endl;
				return !myv.empty() || !active;
			});

			if (!active && myv.empty()) {
				if (v == true) {
					cout << "Задач для " << this_id << " нет" << endl;
				}
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
			func();
			if (v == true) {
				cout.width(10);
				cout << this_id << " Задача " << name << " выполнена " << endl;
			}
		}

		catch (const exception&) {
			cout.width(10);
			cout << this_id << " Вызвано исключение у задачи " << id << endl;
		}
	}
}

