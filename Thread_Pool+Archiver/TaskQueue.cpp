#include "TaskQueue.h"
#include <algorithm>

using namespace std;
using namespace my;

TaskQueue::TaskQueue(int q, bool verbose) {
	v = verbose;
	capacity = q;
}


int TaskQueue::task_id() const {
	return function_queue.top().task_id();
}

string TaskQueue::task_name() const {
	return function_queue.top().task_name();
}

function<void()> TaskQueue::task_f() const {
	return function_queue.top().task_func();
}

bool TaskQueue::empty() const {
	bool val1 = function_queue.empty();
	return val1;
}

void TaskQueue::push(task t) {
	sz = function_queue.size();
	if ((sz + 1) <= static_cast<size_t>(capacity)) {
		unique_lock<mutex> lock_(m_mtx);
		function_queue.push(t);
		function_queue.top();/// нужно чтобы задачи отсортировались по приоритету

		if (v == true) {
			cout.width(10);
			cout << " Добавлена задача " << t.task_id() << " " << t.task_name() << " с приоритетом " << t.task_priority() << endl;
		}
	}
	else {
		if (v == true) {
			cout.width(10);
			cout << " Не могу добавить задачу " << t.task_id() << t.task_name() << " размер очереди " << sz << endl;
		}
	}
}

bool TaskQueue::pop() {
	unique_lock<mutex> lock_(m_mtx);
	if (size_function_queue()> 0) {
		function_queue.pop();
		return true;
	}
	return false;
}

std::size_t TaskQueue::size_function_queue() const {
	return function_queue.size();
}

std::size_t TaskQueue::capacity_() const {
	return capacity;
}

