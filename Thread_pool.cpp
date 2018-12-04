#include "Thread_pool.h"

extern bool v;

Thread_pool::Thread_pool() : function_queue(), lock(), data_condition(), accept_functions(true){}
Thread_pool::~Thread_pool(){}

int error_id = 0;

void Thread_pool::push(int id,std::function<void()> func,int priority,bool doing,string name){
	std::unique_lock<std::mutex> lock(lock);
	task t;
	t.id = id;
	t.f = func;
	t.name = name;
	t.doing = doing;
	t.priority = priority;	
	function_queue.push(t);
	lock.unlock();
	data_condition.notify_one();
}

void Thread_pool::finish(){
	accept_functions = false;//fetch_and_sub
	data_condition.notify_all();
}

void Thread_pool::work(){
	std::function<void()> func;
	string what_i_do;
	int priority = 0;
	task d;
	
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
			func();
			cout << "\nВремя работы = " << clock() / 1000.0 << endl;
		}
		else {
				func();
		}
	}
}