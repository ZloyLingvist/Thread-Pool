#pragma once
#include <fstream>
#include <iostream>

using namespace std;

extern bool v;
extern int p;

struct task {
	int id;
	int priority;
	string name;
	bool doing;
	std::function<void()> f;
	bool operator<(const task &other) const { return priority < other.priority; }
};

class TaskQueue {
private:
	std::priority_queue<task> function_queue;
	size_t sz;
	int quescap;
	std::mutex m_mtx;
	std::condition_variable m_cond;
public:
	task t;
	int r = 0;
	vector<string> myvec;
	vector<std::function<void()>> functions;
	TaskQueue(int q, vector<string> vec, vector<std::function<void()>> func){
		quescap = q; 
		myvec = vec;
		functions = func;
	}

	 void add_task(){
		srand(time(NULL));
		for (int i = 0; i < (1 + rand() % 5); i++) {
			r = rand() % 4;
			t.id = i;
			t.name = myvec[r];
			t.f = functions[r];
			t.doing = false;
			if (p > 1){
				t.priority = 1 + rand() % 100;
			}
			if (p==1){
				t.priority = 0;
			}
			sz = function_queue.size();
			if ((sz + 1) <= quescap) {
				std::unique_lock<std::mutex> lock_(m_mtx);
				function_queue.push(t);
				if (v == true) {
					cout << "\nДобавлена задача " << t.name << " с приоритетом " << t.priority << endl;
				}
				lock_.unlock();
				m_cond.notify_one();
				function_queue.top();
			}
			else {
				if (v == true) {
					cout << "\nНе могу добавить в очередь задачу " << t.name << " текущий размер очереди уже равен " << sz << endl;
				}
			}
		}
	}

	void push_to_end(task d){
		sz = function_queue.size();
		if ((sz + 1) <= quescap) {
			std::unique_lock<std::mutex> lock_(m_mtx);
			function_queue.push(t);
			if (v == true) {
				cout << "\nЗадача " << t.name << " перемещена в конец " << endl;
			}
			lock_.unlock();
			m_cond.notify_one();
		}
		else {
			if (v == true) {
				cout << "\nНе могу добавить в очередь " << " текущий размер очереди " << sz << endl;
			}
		}
	}

	
	void print(int mode, std::thread::id this_id){
		if (mode == 1) {
			cout << "\nПоток " << this_id << " запрашивает задачу" << endl;
		}

		if (mode == 2) {
			cout << "\nПоток " << this_id << " получил задачу" << endl;
		}

		if (mode == 3) {
			cout << "\nПоток " << this_id << " не получил задачу" << endl;
		}
	}

	task give_task(){
		task d=function_queue.top();
		return d;
	}

	bool empty(){
		bool val=function_queue.empty();
		return val;
	}

	void pop(){
		function_queue.pop();
	}

	TaskQueue::~TaskQueue() {}
};


