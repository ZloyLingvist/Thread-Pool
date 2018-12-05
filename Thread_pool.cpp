#pragma once
#include "Thread_pool.h"
#include "TaskQueue.h"

extern bool v;

Thread_pool::Thread_pool(int w) : lock(), data_condition(), accept_functions(true), vector_thread_pool(){}
Thread_pool::~Thread_pool() {
	for (auto &t : vector_thread_pool) {
		t.join();
	}
}

int error_id = 0;
 
void Thread_pool::finish(){
	accept_functions = false;//fetch_and_sub
	data_condition.notify_all();
}

void Thread_pool::init(int w, TaskQueue &obj){
	for (int i = 0; i < 2; i++){
		vector_thread_pool.emplace_back(std::thread(&Thread_pool::work,this, std::ref(obj)));
	}
}

void Thread_pool::work(TaskQueue &obj){
	while (true) {
		task d = obj.give_task();
		std::function<void()> func = d.f;
		func();
	}
}
	/*std::function<void()> func;
	string what_i_do;
	int priority = 0;
	task d;

	while (true) {
		{
			std::unique_lock<std::mutex> lock_(lock);
			data_condition.wait(lock_, [this]() {
				return 0 || !accept_functions;
			});
			if (!accept_functions && obj.empty()) {
				return;
			}

			obj.sort();
			d = obj.give_task();
			func = d.f;
			what_i_do = d.name;
			priority = d.priority;
			obj.pop();
		}

		if (v == true) {
			std::thread::id this_id = std::this_thread::get_id();
			std::cout << "\nНомер потока " << this_id << endl;
			cout << "\nПриоритет задачи: " << priority << endl;
			cout << what_i_do << endl; //что делаю
			srand(time(0));
			func();
			cout << "\nВремя работы = " << clock() / 1000.0 << endl;
		}
		else {
			func();
		}
	}

}

//void Thread_pool::push(TaskQueue &obj){
	//cout << "a + b" << endl;
	//std::unique_lock<std::mutex> lock(lock);
	//task t;
	//t.id = obj.id;
	//t.f = func;
	//t.name = name;
	//t.doing = doing;
	//t.priority = priority;	
	//function_queue.push(t);
	//lock.unlock();
	//data_condition.notify_one();
//}

//void Thread_pool::finish(){
	//accept_functions = false;//fetch_and_sub
	//data_condition.notify_all();
//}

//void Thread_pool::work(TaskQueue &obj){
	
	
	
	
	
	
	
	
	
	

	//std::function<void()> func;
	//string what_i_do;
	//int priority = 0;
	//task d;
	
	//d = obj.give_task();
	//func = d.f;
	//func();
	
	
	/*auto task = task_queue.front_pop();
	while (true){
		{
			std::unique_lock<std::mutex> lock_(lock);
			data_condition.wait(lock_, [this](){ 
				return !function_queue.empty() || !accept_functions; 
			});
			if (!accept_functions && function_queue.empty()){
				return;
			}

			function_queue.top();
			d=function_queue.top();
			func = d.f; 
			what_i_do = d.name;
			priority = d.priority;
			function_queue.pop();
		}

		if (v == true){
			std::thread::id this_id = std::this_thread::get_id();
			std::cout << "\nНомер потока " << this_id << endl;
			cout << "\nПриоритет задачи: " << priority << endl;
			cout << what_i_do << endl; //что делаю
			srand(time(0));
			try {
				func();
				throw std::runtime_error{ "invalid instruction" };
			}
			catch (const std::exception &e) {
				std::cerr << e.what() << std::endl;
				//task_queu.push();
			}
			}
			cout << "\nВремя работы = " << clock() / 1000.0 << endl;
		}
		else {
				func();
		}
	}*/
//}
