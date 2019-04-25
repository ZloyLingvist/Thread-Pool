﻿#include "TaskQueue.h"

TaskQueue::TaskQueue(int q,bool verbose){
	v = verbose;
	quescap = q;
}

void TaskQueue::add_task(const char *name, int p,task obj){
	obj.make_task(name, p);
	task_vector.push_back(obj);
	std::sort(task_vector.begin(), task_vector.end(), [](task &v1, task &v2) {return v1.task_priority() < v2.task_priority(); });
}

void TaskQueue::push_to_end(std::thread::id this_id) {
	bool v = true;
	sz = function_queue.size();
	t = function_queue.top();
	if ((sz + 1) <= quescap) {
		std::unique_lock<std::mutex> lock_(m_mtx);
		function_queue.push(t);
			
		if (v == true){
			std::cout.width(10);
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

void TaskQueue::print(int mode, std::thread::id this_id) {
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

task TaskQueue::give_task() {
	function_queue.top();
	t = function_queue.top();
	return t;
}

int TaskQueue::task_id(){
	return give_task().task_id();
}

string TaskQueue::task_name() {
	return give_task().task_name();
}

std::function<void(int id)> *TaskQueue::task_f(){
	return give_task().task_func();
}

bool TaskQueue::empty(std::thread::id this_id){
	bool val1 = function_queue.empty();
	bool val2 = task_vector.empty();
	return val1;
}

void TaskQueue::push(int k) {
	sz = function_queue.size();
	if ((sz + 1) <= quescap){
		std::unique_lock<std::mutex> lock_(m_mtx);
		t = task_vector.back();
		function_queue.push(t);

		if (check_task_vector() != true) {
			task_vector.pop_back(); /// удалить задачу из пула задач
		}

		//if (v == true) {
			cout.width(10);
			cout << " Добавлена задача " << k << " " << t.task_name() << " с приоритетом " << t.task_priority() << endl;
		//}
			m_cond.notify_one();
	}
	else {
		if (v == true) {
			cout.width(10);
			//cout << this_id << " Íå ìîãó äîáàâèòü â î÷åðåäü çàäà÷ó " << k << t.task_name() << " òåêóùèé ðàçìåð î÷åðåäè óæå ðàâåí " << sz << endl;
		}

		m_cond.notify_one();
	}
}

void TaskQueue::pop() {
	std::unique_lock<std::mutex> lock_(m_mtx);
	if (size_function_queue()> 0){
		function_queue.pop();
	}
	m_cond.notify_one();
}

bool TaskQueue::check_task_vector() {
	return task_vector.empty();
}

int TaskQueue::return_quescap(){
	return quescap;
}

size_t TaskQueue::size_function_queue() {
	return function_queue.size();
}

void TaskQueue::simple_run(){
	int k = 0;
	while (check_task_vector() != true) {
		push(k);
		(*task_f())(task_id());
		std::cout << " Задача " << task_name()<<" выполнена " << std::endl;
		pop();
		k = k + 1;
	}
	cout << "Finished" << endl;
}