#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <future>
#include <mutex>
#include <iostream>
#include <ctime>  
#include <string>

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

template <typename T>
class TaskQueue {
private:
	std::queue<T> q;
	std::mutex mutex;
public:
	bool push(T const & value){
		std::unique_lock<std::mutex> lock(mutex);
		q.push(value);
		return true;
	}
		
	bool pop(T & v){
		std::unique_lock<std::mutex> lock(mutex);
		if (q.empty()){
			return false;
		}

		v = q.front();
		q.pop();
		return true;
	}

	bool empty(){
		std::unique_lock<std::mutex> lock(mutex);
		return q.empty();
	}
};



