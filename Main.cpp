#include "Thread_pool.h"
#include "TaskQueue.h"

int verbose;

void help(){
	cout << "-w -- workers <int> -- число потоков " << endl;
	cout << "-q -- quescap <int> -- объём очереди задач " << endl;
	cout << "-p -- priorities <int> -- число приоритетов задач, если 1--приоритетов нет, >1--если есть приоритет " << endl;
	cout << "-v -- verbose -- логирование " << endl;
	cout << "-h -- help " << endl;
}

int main(int argc, char* argv[]){
	int workers = 0;
	int quescap = 0;
	int priorities = 0;
	int value = 0;
	int i = 1;
	Thread_pool thread_pool;

	setlocale(LC_ALL, "Russian");

	while (i < argc){
		char* par = strtok(argv[i], "=");
		char* val = strtok(NULL, "=");

		if (strcmp(par, "-w") == 0){
			workers = atoi(val);
		}

		if (strcmp(par, "-q") == 0){
			quescap = atoi(val);
		}

		if (strcmp(par, "-p") == 0){
			priorities = atoi(val);
		}

		if (strcmp(par, "-v") == 0){
			verbose = 1;
		}

		if (strcmp(par, "-h") == 0) {
			help();
			exit(0);
		}

		i = i + 1;
	}

	if (workers == 0){
		workers = std::thread::hardware_concurrency() + 1; //запускаем на максимально возможном для системы количестве потоков
	}

	if (quescap == 0){ //минимум 1 задача
		quescap = 1;
	}

	if (priorities == 0){ //по умолчанию нет приоритетов
		priorities = 1;
	}

	std::vector<std::thread> vector_thread_pool;
	for (int i = 0; i < workers; i++){
		vector_thread_pool.push_back(std::thread(&Thread_pool::work, &thread_pool));
	}

	for (int i = 0; i < quescap; i++){
		if (priorities == 1) {
			value = 0;
		}
		else {
			value = quescap;
		}

		thread_pool.push(1, example_function1, value);
		thread_pool.push(2, example_function2, value);
	}
	
	thread_pool.finish();

	for (unsigned int i = 0; i < vector_thread_pool.size(); i++){
		vector_thread_pool.at(i).join();
	}

	return 0;
}