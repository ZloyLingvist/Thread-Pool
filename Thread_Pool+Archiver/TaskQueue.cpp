#include "TaskQueue.h"

TaskQueue::TaskQueue(int q,bool verbose){
	v = verbose;
	quescap = q;
}

void TaskQueue::add_task(const char *name, int p, int workday,task d) {
	d.name = name;
	d.priority = p;
	d.workday = workday;
	task_vector.push_back(d);
	std::sort(task_vector.begin(), task_vector.end(), [](const task &v1, const task &v2) {return v1.priority < v2.priority; });
}

bool TaskQueue::run(std::thread::id this_id) {
	srand(time(NULL));
	int k = 0;

	if (task_vector.empty() == true) {
		return false;
	}

	for (int i = 0; i < (1 + rand() % quescap); i++){
		sz = function_queue.size();
		if ((sz + 1) <= quescap) {
			std::unique_lock<std::mutex> lock_(m_mtx);
			task t;
			t = task_vector.back();
			function_queue.push(t);
			
			if (task_vector.empty() != true){
				task_vector.pop_back();
			}

			if (v == true) {
				cout.width(10);
				cout << this_id << " ƒобавлена задача " << k << " " << t.name << " с приоритетом " << t.priority << endl;
			}

			m_cond.notify_one();
		}
		else {
			if (v == true) {
				cout.width(10);
				cout << this_id << " Ќе могу добавить в очередь задачу " << k << t.name << " текущий размер очереди уже равен " << sz << endl;
			}
			
			m_cond.notify_one();
		}

		k = k + 1;
	}

	function_queue.top();
	return true;
}

void TaskQueue::push_to_end(task d, std::thread::id this_id) {
	bool v = true;
	sz = function_queue.size();
	if ((sz + 1) <= quescap) {
		std::unique_lock<std::mutex> lock_(m_mtx);
		function_queue.push(t);
			
		if (v == true){
			std::cout.width(10);
			cout << this_id << " «адача " << t.id_ << " перемещена в конец " << endl;
		}

		lock_.unlock();
		m_cond.notify_one();
	}
	
	else {
		if (v == true){
			cout << " Ќе могу добавить в очередь задачу " << t.id_ << " текущий размер очереди " << sz << endl;
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
			cout << "ѕоток " << this_id << " запрашивает задачу" << endl;
		}

		if (mode == 2) {
			cout.width(10);
			cout << "ѕоток " << this_id << " получил задачу" << endl;
		}

		if (mode == 3) {
			cout.width(10);
			cout << "ѕоток " << this_id << " не получил задачу" << endl;
		}
	}

task TaskQueue::give_task() {
	function_queue.top();
	task d = function_queue.top();
	return d;
}

bool TaskQueue::empty() {
	bool val = function_queue.empty();
	return val;
}

void TaskQueue::pop() {
	std::unique_lock<std::mutex> lock_(m_mtx);
	function_queue.pop();
	m_cond.notify_one();
}
