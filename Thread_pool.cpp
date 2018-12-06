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
		vector_thread_pool.emplace_back(std::thread(&Thread_pool::work,this, std::ref(obj)));
	}
}

void Thread_pool::work(TaskQueue &obj){
	std::function<void()> func;
	task d;
	int error = 0;
	int id = 0;
	int error_id=0;
	std::thread::id this_id = std::this_thread::get_id();

	while (true) {
		{
			std::unique_lock<std::mutex> lock_(lock);
			
			data_condition.wait(lock_,[this,&obj](){
				//if (v == 1){
					//std::this_thread::sleep_for(0.5s);
				//}
				return !obj.empty() || !active;
			});

			if (!active && obj.empty()) {
				obj.print(1, this_id);
				obj.add_task();
				error = 0;
				obj.print(4, this_id);
			}

			d = obj.give_task();
			id = d.id;
			if (error < 2 && error_id != id) {
				func = d.f;
			}
			else {
				obj.pop();//удаляем плохую задачу
				obj.add_task();//генерируем новую
				d = obj.give_task();
				id = d.id;
				func = d.f;
				error = 0;//обнуляем счетчик ошибок
			}

			obj.pop();
			active = false;
			data_condition.notify_all();
		}

		try {
			func();
		}
		catch (const std::exception &e) {
			std::cout << " Вызвано исключение у задачи "<<d.name << " c номером "<< d.id << std::endl;
			obj.push_to_end(d);
			error_id = id;
			error = error + 1;
		}	
	}
}
	