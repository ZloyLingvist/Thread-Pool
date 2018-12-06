#pragma once
#include "Thread_pool.h"
#include "TaskQueue.h"

extern bool v;

Thread_pool::Thread_pool(int w) : lock(), data_condition(), active(true), vector_thread_pool(){ workers = w; }
Thread_pool::~Thread_pool(){
	for (auto &t : vector_thread_pool){
		t.join();
	}
}

/*--- Закидываем в вектор треды ---*/
void Thread_pool::init(int workers, TaskQueue &obj){
	for (int i = 0; i < workers; i++){
		vector_thread_pool.emplace_back(std::thread(&Thread_pool::work,this,std::ref(obj)));
	}
}

void Thread_pool::work(TaskQueue &obj){
	std::function<void()> func;
	task d;
	int error = 0;
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
				obj.add_task();
				error = 0;
				obj.print(4, this_id);
			}

			d = obj.give_task();
			name = d.name;

			if (error < 2 && error_name != name) {
				func = d.f;
				obj.pop();
			}
			else {
				obj.pop();//удаляем плохую задачу
				obj.add_task();//генерируем новую
				d = obj.give_task();
				name = d.name;
				func = d.f;
				error = 0;//обнуляем счетчик ошибок
			}

			active = false;
			data_condition.notify_all();
		}

		try {
			func();
			if (v == true) {
				std::cout << "\nЗадача " << d.name << " c приоритетом " << d.priority << " выполнена " << std::endl;
			}
		}
		catch (const std::exception &e) {
			if (v == true){
				std::cout << "\n----Вызвано исключение у задачи " << d.name << " c приоритетом " << d.priority << std::endl;
			}
			obj.push_to_end(d);
			error_name = name;
			error = error + 1;
		}	
	}
}
	