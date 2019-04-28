#include "tester.h"
#include "TaskQueue.h"
#include "Threadpool.h"

bool Testing_class::test_1(int p){
	if (p == 1) {
		logging("--[Archiver creation test]--");
	}

	creation_function(p,log_test);
	string my(test_vector[0]);
	logging(("Archive "+my).c_str());

	if (exists(test_vector[0].c_str()) == false){
		return false;
	}

	size_before = 0;
	cleaner();
	return true;
}

bool Testing_class::test_2(int p) {
	if (p == 1) {
		creation_function(4,log_test);
	}
	else {
		if (p == 2) {
			logging("--[Test on extraction]--");
		}
		creation_function(p,log_test);
	}

	string my(test_vector[0]);
	logging(("Archive " + my).c_str());
	
	bool val = extraction_function(p,log_test);
	
	if (val == false) {
		return false;
	}

	if (size_after != size_before) {
		return false;
	}

	cleaner();
	return true;
}
bool Testing_class::sub_test_3() {
	logging("User_test");

	creation_function(0,log_test);
	
	if (exists(test_vector[0].c_str()) == false) {
		return false;
	}

	bool val = extraction_function(0, log_test);

	if (val == false) {
		return false;
	}

	if (size_after!=size_before){
		return false;
	}

	cleaner();
	return true;
}

bool Testing_class::test_5(const char* in, const char* out) {
	lib_archiver obj;
	obj.compress(in,"tmp_lib.txt");
	obj.decompress("tmp_lib.txt",out);
	bool v = isFilesEqual(in, out);
	return true;
}


bool Testing_class::test_7(int q) {
	bool v = false;
	TaskQueue queue(q, v);
	logging("--[TestQueue without ThreadPool]--");

	try {
		function_block(queue, log_test);
		queue.simple_run();
		return true;
	}
	catch (const std::exception &e) {
		return false;
	}
}

bool Testing_class::test_8(int w) {
	bool v = false;
	logging("--[ThreadPool without TestQueue ]--");
	try {
		Threadpool thread_pool(w,v);
		thread_pool.add_function(void_task,5,4,40,"stroka");
		thread_pool.add_function(int_task,10,200);
		thread_pool.add_function(string_test, "10", "200");
		return true;
	}
	catch (const std::exception &e) {
		return false;
	}
}


bool Testing_class::test_9(int w, int q){
	logging("--[TaskQueue + Thread_pool(simple)]--");
	try {
		task t1, t2, t3,t4;
		bool v = false;
		t1.add_function(write, "T1\n", 1);
		t2.add_function(write, "T2\n", 1);
		t3.add_function(write, "T3\n", 1);
		t4.add_function(write, "T4\n", 1);

		TaskQueue queue(q, v);
		queue.add_task("T1", 3, t1);
		queue.add_task("T2", 2, t2);
		queue.add_task("T3", 1, t3);
		queue.add_task("T4", 0, t4);

		Threadpool thread_pool(w, queue, v);
		return true;
	}

	catch (const std::exception &e) {
		return false;
	}
}

bool Testing_class::test_10(int w,int q){
	logging("--[TaskQueue + Thread_pool+Archiver]--");
	try {
		bool v = false;
		TaskQueue queue(q, v);
		function_block(queue, log_test);
		Threadpool thread_pool(w, queue, v);
		return true;
	}

	catch (const std::exception &e) {
		return false;
	}
}

void Testing_class::logging(const char *msg) {
	if (log_test == 0) {
		ofstream fout_log_test;
		fout_log_test.open("log_test.txt", std::ios_base::app);
		fout_log_test << msg << endl;
		fout_log_test.close();
	}

	if (log_test == 1) {
		cout << msg << endl;
	}
}


void comp_sub(const char *file1, const char *file_res1,int log) {
	Testing_class obj(log);
	if (obj.isFilesEqual(file1, file_res1) == true) {
		cout << file1 << "~" << file_res1 << endl;
	}
	else {
		cout << file1 << "not ~" << file_res1 << endl;
	}
}
void comp(int i, const char *file1, const char *file2, const char *file_res1, const char *file_res2) {
	comp_sub(file1, file_res1,1);
	comp_sub(file2, file_res2,1);
}