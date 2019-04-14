#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include <time.h>
#pragma comment ( lib, "getopt.lib" )
extern "C" {
#include "getopt.h"   /* for getopt_long function */
}

bool v = false;
int p = 1;

/*------------ Задачи ---------------*/
int archive_test() {
	/* создали архив */
	cout << "Make archive" << endl;
	Tar tar("archive.tar");
	ifstream inFile;
	ofstream outFile;
	inFile.open("r.txt");
	outFile.open("t.txt");
	RLECompression* compression = new RLECompression(0, inFile, outFile);
	inFile.close();
	outFile.close();
	tar.add_to_archive("t.txt", "ct.txt");
	remove("t.txt");
	tar.close();
	tar.extract(string("archive.tar"));
	inFile.open("ct.txt");
	outFile.open("res.txt");
	RLECompression* compression2 = new RLECompression(1, inFile, outFile);
	inFile.close();
	outFile.close();

	if (isFilesEqual("r.txt", "res.txt") == true) {
		cout << "equivalent";
	}
	else {
		cout << "not equivalent" << endl;
	}

	system("pause");
	return 0;
}

void example_function1() {
	int i, j;
	//srand(time(NULL));
	int n = 1 +  rand()% 20;

	double *a;

	a = new double[n];
	srand(time(0));
	for (int i = 0; i < n; i++) {
		a[i] = 1 + rand() % 100;
	}

	//if (v == true) {
		/*cout << "\nЧто сортируем:" << endl;
		for (int i = 0; i < n; i++) {
			cout << a[i] << " ";
		}

		cout << endl;*/
	//}

	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (a[j] > a[j + 1]) {
				int temp = a[j];
				a[j] = a[j + 1];
				a[j + 1] = temp;
			}
		}
	}

	//cout << "\nВремя работы генерации и сортировки = " << clock() / 1000.0 << endl;

	/*if (v == true) {
		cout << "\nРезультат сортировки" << endl;
		for (int i = 0; i < n; i++) {
			cout << a[i] << " ";
		}

		cout << "\n*****" << endl;
	}*/

	_sleep(200);
}

/*---- Функция ошибки ----*/
void error_func() {
	throw std::invalid_argument("Ошибка была здесь");
}

void example_function2() {
	//srand(time(0));
	ofstream fout("test.txt");
	for (int i = 0; i < 500; i++) {
		fout << "\nПишу в файл " << endl; // запись строки в файл
	}
	fout.close();
	//cout << "\nВремя работы Записи в файл = " << clock() / 1000.0 << endl;

	/*if (v == true) {
		cout << "\nПишу на экран " << endl;
		for (int i = 0; i < 500; i++) {
			cout << i << " "; // запись строки на экран
		}

		cout << endl;
		cout << "\n-----" << endl;
	}*/

	_sleep(300);
}

void example_function3() {
	int n = 1 + rand() % 20;
	double **a, **b, **c;

	a = new double*[n];
	//srand(time(NULL));
	for (int i = 0; i < n; i++) {
		a[i] = new double[n];
		for (int j = 0; j < n; j++) {
			a[i][j] = 1 + rand() % 10;
		}
	}

	b = new double*[n];
	for (int i = 0; i < n; i++) {
		b[i] = new double[n];
		for (int j = 0; j < n; j++) {
			b[i][j] = 1 + rand() % 10;
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

	//cout << "\nВремя работы генерации и умножения матриц = " << clock() / 1000.0 << endl;
	/*if (v == true) {
	cout << "\nРезультат умножения матриц" << endl;
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++) {
			cout << c[i][j] << " ";
		}
		cout << endl;
	}

	cout << "\n+++++" << " ";
	cout << endl;
	}*/

	delete c;
	_sleep(400);
}



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




int main(int argc, char* argv[]) {
	AppArgs args;
	setlocale(LC_ALL, "Russian");

	/* Разбор параметров командной строки */
	parseConsoleArguments(argc, argv, args);

	if (args.w == 0){
		args.w = 1;
	}

	if (args.q == 0){ //минимум 1 задача
		args.q = 1;
	}

	if (args.p == 0){ //по умолчанию нет приоритетов
		args.p = 1;
	}

	v = args.verbose;
	p = args.p;
	/*vector<string> arr = { "Сортировка массива" ,"Проверка на ошибку","Запись в файл" , "Умножение матриц","Архиватор"};
	vector<std::function<void()>> functions={ example_function1, error_func,example_function2, example_function3,archive_test};
	
	TaskQueue queue(args.q,arr,functions);
	std::thread::id main_thread_id = std::this_thread::get_id();
	queue.add_task(main_thread_id);
	
	Thread_pool thread_pool(args.w);
	thread_pool.init(args.w, queue);*/
	archive_test();
	return 0;
}
