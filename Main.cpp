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
		"w -- workers <int> -- ����� ������� " 
		"q -- quescap <int> -- ����� ������� ����� " 
		"p -- priorities <int> -- ����� ����������� �����, ���� 1--����������� ���, >1--���� ���� ��������� ";
		"v -- verbose -- ����������� ";
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
	int amount = 50;//���������� �����,������� ����� ������
	task t;
	AppArgs args;
	Thread_pool thread_pool;
	std::vector<std::thread> vector_thread_pool;
	std::vector<task> vector_queue;
	vector_queue.reserve(amount);

	setlocale(LC_ALL, "Russian");

	/* ������ ���������� ��������� ������ */
	parseConsoleArguments(argc, argv, args);

	if (args.w == 0) {
		args.w = std::thread::hardware_concurrency() + 1; //��������� �� ����������� ��������� ��� ������� ���������� �������
	}

	if (args.q == 0) { //������� 1 ������
		args.q = 1;
	}

	if (args.p == 0) { //�� ��������� ��� �����������
		args.p = 1;
	}

	v = args.verbose;

	//// ��������� ������ ����� ��������, ������� ����� ��������� ///
	/// ���������� �������� ///
	/// ���� ������� ���������, �� � �� ������������ �������� ///

	/* ������� ���� ������ � �������� �� ���� */
	//t.id = 0;
	//t.doing = false;
	//t.f = error_func;
	//t.priority = 10;
	//t.name = "������� �� ����";
	//vector_queue.push_back(t);
	/* ----------------------------------------*/

	for (int i = 0; i < amount; i++){
		t.id = i;
		t.doing = false;
		int r = 1+rand() % 3;
		
		if (r==1){
				t.name = "\n�������� ������\n";
				t.f = example_function1;
				if (args.p > 1) {
					t.priority = 1 + rand() % 10;
				}
				else {
					t.priority = 0;
				}
		}

		if (r==2){
				t.name= "\n���� � ��������� ����\n";
				t.f = example_function2;
				if (args.p > 1) {
					t.priority = 1 + rand() % 10;
				}
				else {
					t.priority = 0;
				}
		}

		if (r == 3){
				t.name = "\n������� �������\n";
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

	/* ------------ ������� ������ ----------------*/
	for (int i = 0; i < args.w; i++){
		vector_thread_pool.push_back(std::thread(&Thread_pool::work, &thread_pool));
	}
	/*----------------------------------------------*/
	
	while(i<amount){
		/* ���� �����, ��� ���������� � ������� ���������� ����� ������ ����� �������*/
		/* ���� ��� ������ ������, ���� ��� ������� �� .. */
		/* �� ���� ��� ���� ��������� ����� ��������� ������, ���� ��� ������� �������� ��� */
		
		for (int k = 0; k < args.q; k++){ 
			try {
				thread_pool.push(vector_queue[k].id, vector_queue[k].f, vector_queue[k].priority, vector_queue[k].doing, vector_queue[k].name);
			} 
			catch (...) {
				/* ������, ��� ����� ������ ���������� ��� ������ ������ �� 0 
				/*	� ����� ����� ��� � ������ ���������� � �������	*/
				if (args.verbose == true){
					cout << "������ ��� ����������" << endl;
				}
				vector_queue.push_back(vector_queue[i+k]);
				vector_queue.erase(vector_queue.begin()+i+k);
				error = error + 1;
			}
		}

		i = i + args.q;
	}

	/*------------------ ��������� ������----------------------- */
	thread_pool.finish();
	for (unsigned int i = 0; i < vector_thread_pool.size(); i++){
		vector_thread_pool.at(i).join();
	}

	return 0;
}