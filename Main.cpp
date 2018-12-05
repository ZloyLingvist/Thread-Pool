#pragma once
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

struct AppArgs {
	std::string s;
	int w;
	int p;
	int q;
	bool verbose = false;
};
void parseConsoleArguments(int argc, char **argv, AppArgs &args);

void parseConsoleArguments(int argc, char **argv, AppArgs &args) {
	const char *help_text =
		"w -- workers <int> -- число потоков "
		"q -- quescap <int> -- длина очереди "
		"p -- priorities <int> -- приоритет ";
		"v -- verbose --логирование ";
		"h -- help ";

	const char *args_pattern = "w:q:p:v";
	static struct option long_options[] = {
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
	if (optind < argc) {
		cout << "Non-option ARGV-elements: " << endl;
		while (optind < argc)
			cout << argv[optind++] << endl;
		cout << endl;
	}

	return;
}

void example_function1() {
	int i, j;
	int n = 1 + rand() % 100;

	double *a;

	a = new double[n];
	for (int i = 0; i < n; i++) {
		a[i] = 1 + rand() % 100;
	}

	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (a[j] > a[j + 1]) {
				int temp = a[j];
				a[j] = a[j + 1];
				a[j + 1] = temp;
			}
		}
	}

	//if (v == true) {
		for (int i = 0; i < n; i++) {
			cout << a[i] << " ";
		}
	//}
}


void error_func() {
	throw std::invalid_argument("Ошибка была здесь");
}

void example_function2() {
	ofstream fout("test.txt");
	for (int i = 0; i < 100; i++) {
		fout << "Пишу в файл " << endl; // запись строки в файл
	}
	fout.close();

	for (int i = 0; i < 100; i++) {
		fout << "Пишу на экран" << endl; // запись строки на экран
	}

}

void example_function3() {
	int n = 1 + rand() % 20;

	double **a, **b, **c;

	a = new double*[n];
	for (int i = 0; i < n; i++) {
		a[i] = new double[n];
		for (int j = 0; j < n; j++) {
			a[i][j] = 1 + rand() % 20;
		}
	}

	b = new double*[n];
	for (int i = 0; i < n; i++) {
		b[i] = new double[n];
		for (int j = 0; j < n; j++) {
			b[i][j] = 1 + rand() % 20;
		}
	}

	c = new double*[n];
	for (int i = 0; i<n; i++)
	{
		c[i] = new double[n];
		for (int j = 0; j<n; j++)
		{
			c[i][j] = 0;
			for (int k = 0; k<n; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	}

	if (v == true) {
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
				cout << c[i][j] << " ";
			cout << endl;
		}
	}
}



int main(int argc, char* argv[]) {
	int error = 0;
	int i = 0;
	int r = 0;
	task t;
	AppArgs args;
	
	setlocale(LC_ALL, "Russian");

	/* Разбор параметров командной строки */
	parseConsoleArguments(argc, argv, args);

	if (args.w == 0) {
		args.w = 1;
	}

	if (args.q == 0) { //минимум 1 задача
		args.q = 1;
	}

	if (args.p == 0) { //по умолчанию нет приоритетов
		args.p = 1;
	}

	v = args.verbose;

	//void(*funcs[4])(void) = { example_function1, error_func,example_function2, example_function3};
	string arr[4] = { "Сортировка массива" ,"Проверка на ошибку","Запись в файл" , "Умножение матриц" };
	TaskQueue queue(1);

	for (int i = 0; i < (1 + rand()%5); i++) {
		r = rand()%4;
		t.name = arr[r];
		t.f = example_function1;
		t.doing = false;
		if (args.p > 1){
				t.priority = 1 + rand() % 10;
		}
		else {
			t.priority = 0;
		}

		queue.push(t);
	}


	Thread_pool thread_pool(args.w);
	thread_pool.work(queue);



	/* ------------ Создаем потоки ----------------*/
	//for (int i = 0; i < args.w; i++){
		//vector_thread_pool.push_back(std::thread(&Thread_pool::work, &thread_pool));
	//}
	/*----------------------------------------------*/
	
	/*while(i<amount){
		/* Идея такая, что отправляем в тредпул количество задач равное длине очереди*/
		/* Если все задачи решены, дать еще столько же .. */
		/* Но идеи как тред свободный может запросить задачу, пока его коллеги работают нет */
		
		//кидать задачу ё
		//for (int k = 0; k < args.q; k++){ 
			//try {
				//thread_pool.push(vector_queue[k].id, vector_queue[k].f, vector_queue[k].priority, vector_queue[k].doing, vector_queue[k].name);
			//} 
			//catch (...) {
				/* Неясно, где нужно ловить исключение для задачи делить на 0 
				/*	в самом треде или в момент добавления в очередь	*/
				//if (args.verbose == true){
					//cout << "Ошибка при добавлении" << endl;
				//}
				//vector_queue.push_back(vector_queue[i+k]);
				//vector_queue.erase(vector_queue.begin()+i+k);
				//error = error + 1;
			//}
		//}

		//i = i + args.q;
	//}

	/*------------------ Закончили работу----------------------- */
	//thread_pool.finish();
	//for (unsigned int i = 0; i < vector_thread_pool.size(); i++){
		//vector_thread_pool.at(i).join();
	//}*/

	return 0;
}
