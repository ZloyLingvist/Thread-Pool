#include "Thread_pool.h"

extern int verbose;

Thread_pool::Thread_pool() : function_queue(), lock(), data_condition(), accept_functions(true){}
Thread_pool::~Thread_pool(){}

void Thread_pool::push(int id,std::function<void()> func,int priority,string doing){
	std::unique_lock<std::mutex> lock(lock);
	task t;
	t.id = id;
	t.f = func;
	t.doing = doing;
	t.priority = priority;	
	function_queue.push(t);
	lock.unlock();
	data_condition.notify_one();
}

void Thread_pool::finish(){
	std::unique_lock<std::mutex> lock(lock);
	accept_functions = false;
	lock.unlock();
	data_condition.notify_all();
}

void Thread_pool::work(){
	std::function<void()> func;
	string what_i_do;
	
	while (true){
		{
			std::unique_lock<std::mutex> lock(lock);
			data_condition.wait(lock, [this](){ 
				return !function_queue.empty() || !accept_functions; 
			});
			if (!accept_functions && function_queue.empty()){
				return;
			}

			function_queue.top();
			task d=function_queue.top();
			func = d.f;
			what_i_do = d.doing;
			function_queue.pop();
		}

		if (verbose == 1){
			std::thread::id this_id = std::this_thread::get_id();
			srand(time(0));
			std::cout << "Номер потока " << this_id << endl;
			cout << what_i_do << endl;
			func();
			cout << "Время работы = " << clock() / 1000.0 << endl;
		}
		else {
			func();
		}
	}
}