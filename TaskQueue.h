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
	int k = 0;
	vector<string> myvec;
	vector<std::function<void()>> functions;
	TaskQueue(int q, vector<string> vec, vector<std::function<void()>> func){
		quescap = q; 
		myvec = vec;
		functions = func;
	}

	int now(){
		return k;
	 }

	 void add_task(std::thread::id this_id){
		srand(time(NULL));
		for (int i = 0; i < (1+rand() % quescap); i++) {
			r = rand() % 4;
			t.id = k;
			t.name = myvec[r];
			t.f = functions[r];
			t.doing = true;
			if (p > 1){
				t.priority = 1 + rand() % p;
			}
			if (p==1){
				t.priority = 1;
			}
			sz = function_queue.size();
			if ((sz + 1) <= quescap) {
				std::unique_lock<std::mutex> lock_(m_mtx);
				function_queue.push(t);
				
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
		_sleep(100*(rand()%100));
	}

	void push_to_end(task d, std::thread::id this_id){
		sz = function_queue.size();
		if ((sz + 1) <= quescap) {
			std::unique_lock<std::mutex> lock_(m_mtx);
			function_queue.push(t);
			if (v == true) {
				cout.width(10);
				cout << this_id << " «адача " << t.id << " перемещена в конец " << endl;
			}
			lock_.unlock();
			m_cond.notify_one();
		}
		else {
			if (v == true) {
				cout << " Ќе могу добавить в очередь задачу " << t.id << " текущий размер очереди " << sz << endl;
			}
		}
		function_queue.top();
	}

	
	void print(int mode, std::thread::id this_id){
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

	task give_task(){
		function_queue.top();
		task d=function_queue.top();
		return d;
	}

	bool empty(){
		bool val=function_queue.empty();
		return val;
	}

	void pop(){
		std::unique_lock<std::mutex> lock_(m_mtx);
		function_queue.pop();
		m_cond.notify_one();
	}

	TaskQueue::~TaskQueue() {}
};


