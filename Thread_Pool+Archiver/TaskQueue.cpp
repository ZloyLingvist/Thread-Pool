#include "TaskQueue.h"
#include <algorithm>

using namespace std;
using namespace my;

TaskQueue::TaskQueue(int q,bool verbose){
	v = verbose;
	quescap = q;
}

void TaskQueue::add_task(const char *name, int p,task obj){
	obj.make_task(name, p);
	task_vector.push_back(obj);
	sort(task_vector.begin(), task_vector.end(), [](task &v1, task &v2) {return v1.task_priority() < v2.task_priority(); });
}

void TaskQueue::push_to_end(thread::id this_id) {
	bool v = true;
	sz = function_queue.size();
	t = function_queue.top();
	if ((sz + 1) <= static_cast<size_t>(quescap)) {
		unique_lock<mutex> lock_(m_mtx);
		function_queue.push(t);
			
		if (v == true){
			cout.width(10);
			cout << this_id << " Задача " << t.task_id() << " перемещена в конец " << endl;
		}

		lock_.unlock();
		m_cond.notify_one();
	}
	
	else {
		if (v == true){
			cout << " Не могу добавить в очередь задачу " << t.task_id() << " текущий размер очереди " << sz << endl;
		}
	}

	function_queue.top();
}

void TaskQueue::print(int mode, thread::id this_id) const
{
		if (v == false) {
			return;
		}

		if (mode == 1) {
			cout.width(10);
			cout << "Поток " << this_id << " запрашивает задачу" << endl;
		}

		if (mode == 2) {
			cout.width(10);
			cout << "Поток " << this_id << " получил задачу" << endl;
		}

		if (mode == 3) {
			cout.width(10);
			cout << "Поток " << this_id << " не получил задачу" << endl;
		}
	}

task TaskQueue::give_task() const {
	function_queue.top();
	t = function_queue.top();
	return t;
}

int TaskQueue::task_id() const {
	return give_task().task_id();
}

string TaskQueue::task_name() const {
	return give_task().task_name();
}

function<void()> TaskQueue::task_f() const {
	return give_task().task_func();
}

bool TaskQueue::empty() const {
	bool val1 = function_queue.empty();
	return val1;
}

void TaskQueue::push(int k) {
	sz = function_queue.size();
	if ((sz + 1) <= static_cast<size_t>(quescap)){
		unique_lock<mutex> lock_(m_mtx);
		if (check_task_vector() != true) {
			t = task_vector.back();
			function_queue.push(t);
			task_vector.pop_back(); 
		}

		if (v == true) {
			cout.width(10);
			cout << " Добавлена задача " << k << " " << t.task_name() << " с приоритетом " << t.task_priority() << endl;
		}
		
		m_cond.notify_one();
	}
	else {
		if (v == true) {
			cout.width(10);
			cout << " Не могу добавить задачу " << k << t.task_name() << " размер очереди " << sz << endl;
		}

		m_cond.notify_one();
	}
}

void TaskQueue::pop() {
	unique_lock<mutex> lock_(m_mtx);
	if (size_function_queue()> 0){
		function_queue.pop();
	}
	m_cond.notify_one();
}

bool TaskQueue::check_task_vector() const {
	return task_vector.empty();
}

int TaskQueue::return_quescap(){
	return quescap;
}

std::size_t TaskQueue::size_function_queue() const {
	return function_queue.size();
}

void TaskQueue::simple_run(){
	int k = 0;
	while (check_task_vector() != true) {
		push(k);
		task_f()(/*task_id()*/);
		if (v == true){
			cout.width(10);
			cout << " Задача " << (k + 1) << " " << task_name() << " выполнена " << endl;
		}
		pop();
		k = k + 1;
	}
}