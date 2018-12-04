#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#pragma comment ( lib, "getopt.lib" )
extern "C" {
#include "getopt.h"   /* for getopt_long function */
}

bool v = false;

extern int error_id;

struct AppArgs{
	std::string s;
	int w;
	int p;
	int q;
	bool verbose = false;
};
void parseConsoleArguments(int argc, char **argv, AppArgs &args);

void parseConsoleArguments(int argc, char **argv, AppArgs &args){
	const char *help_text =
		"w -- workers <int> -- число потоков " 
		"q -- quescap <int> -- объём очереди задач " 
		"p -- priorities <int> -- число приоритетов задач, если 1--приоритетов нет, >1--если есть приоритет ";
		"v -- verbose -- логирование ";
		"h -- help ";

	const char *args_pattern = "w:q:p:v";
	static struct option long_options[] ={
		{ "quescap",     required_argument, 0, 'q' },
		{ "workers",    required_argument, 0, 'w' },
		{ "verbose",       no_argument, 0, 'v' },
		{ "priorities",         required_argument, 0, 'p' },
		{ "help",          no_argument, 0, 'h' },
		{ "help",          no_argument, 0, '?' },
		{ 0, 0,                        0,  0 }
	};
	
	int c, option_index = 0;
	while (-1 != (c = getopt_long(argc, argv, args_pattern,
		long_options, &option_index)))
	{
		switch (c)
		{
		case -1:
			break;

		case 0:
			switch (option_index)
			{
			case 0: args.w = atoi(optarg);       break;
			case 1: args.q = atoi(optarg); break;
			case 2: args.p = atoi(optarg);   break;
			case 3: args.verbose = true;   break;
			default: // help
				std::printf("%s\n", help_text);
				exit(1);
				break;
			}
			break;

		case 'w': args.w = atoi(optarg);        break;
		case 'q': args.q = atoi(optarg);  break;
		case 'p': args.p = atoi(optarg);  break;
		case 'v': args.verbose = true;    break;
		default: // case '?': case 'h': 
				 /* getopt_long already printed an error message. */
			std::printf("%s\n", help_text);
			exit(1);
			break;
		}
	}
	//-----------------------
	if (optind < argc){
		cout << "Non-option ARGV-elements: " << endl;
		while (optind < argc)
			cout << argv[optind++] << endl;
		cout << endl;
	}

	return;
}

int main(int argc, char* argv[]) {
	int error = 0;
	int i = 0;
	int amount = 50;//количество задач,которое нужно решить
	task t;
	AppArgs args;
	Thread_pool thread_pool;
	std::vector<std::thread> vector_thread_pool;
	std::vector<task> vector_queue;
	vector_queue.reserve(amount);

	setlocale(LC_ALL, "Russian");

	/* Разбор параметров командной строки */
	parseConsoleArguments(argc, argv, args);

	if (args.w == 0) {
		args.w = std::thread::hardware_concurrency() + 1; //запускаем на максимально возможном для системы количестве потоков
	}

	if (args.q == 0) { //минимум 1 задача
		args.q = 1;
	}

	if (args.p == 0) { //по умолчанию нет приоритетов
		args.p = 1;
	}

	v = args.verbose;

	//// Наполняем вектор всеми задачами, которые нужно выполнить ///
	/// Генерируем рандомно ///
	/// Если включен приоритет, то и он генерируется рандомно ///

	/* Создаем одну задачу с делением на ноль */
	//t.id = 0;
	//t.doing = false;
	//t.f = error_func;
	//t.priority = 10;
	//t.name = "Деление на нуль";
	//vector_queue.push_back(t);
	/* ----------------------------------------*/

	for (int i = 0; i < amount; i++){
		t.id = i;
		t.doing = false;
		int r = 1+rand() % 3;
		
		if (r==1){
				t.name = "\nСортирую массив\n";
				t.f = example_function1;
				if (args.p > 1) {
					t.priority = 1 + rand() % 10;
				}
				else {
					t.priority = 0;
				}
		}

		if (r==2){
				t.name= "\nПишу в текстовый файл\n";
				t.f = example_function2;
				if (args.p > 1) {
					t.priority = 1 + rand() % 10;
				}
				else {
					t.priority = 0;
				}
		}

		if (r == 3){
				t.name = "\nУмножаю матрицы\n";
				t.f = example_function3;
				if (args.p > 1) {
					t.priority = 1 + rand() % 10;
				}
				else {
					t.priority = 0;
				}
		}

		vector_queue.push_back(t);
	}

	/* ------------ Создаем потоки ----------------*/
	for (int i = 0; i < args.w; i++){
		vector_thread_pool.push_back(std::thread(&Thread_pool::work, &thread_pool));
	}
	/*----------------------------------------------*/
	
	while(i<amount){
		/* Идея такая, что отправляем в тредпул количество задач равное длине очереди*/
		/* Если все задачи решены, дать еще столько же .. */
		/* Но идеи как тред свободный может запросить задачу, пока его коллеги работают нет */
		
		for (int k = 0; k < args.q; k++){ 
			try {
				thread_pool.push(vector_queue[k].id, vector_queue[k].f, vector_queue[k].priority, vector_queue[k].doing, vector_queue[k].name);
			} 
			catch (...) {
				/* Неясно, где нужно ловить исключение для задачи делить на 0 
				/*	в самом треде или в момент добавления в очередь	*/
				if (args.verbose == true){
					cout << "Ошибка при добавлении" << endl;
				}
				vector_queue.push_back(vector_queue[i+k]);
				vector_queue.erase(vector_queue.begin()+i+k);
				error = error + 1;
			}
		}

		i = i + args.q;
	}

	/*------------------ Закончили работу----------------------- */
	thread_pool.finish();
	for (unsigned int i = 0; i < vector_thread_pool.size(); i++){
		vector_thread_pool.at(i).join();
	}

	return 0;
}