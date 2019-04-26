#include "tester.h"
#include "TaskQueue.h"
#include "Thread_pool.h"

bool Testing_class::test_1(int p){
	if (p == 1) {
		cout << endl << "--[Archiver creation test]--" << endl;
	}

	creation_function(p);
	cout << "Archive " << test_vector[0] << endl;
	if (exists(test_vector[0].c_str()) == false){
		return false;
	}

	size_before = 0;
	test_vector.clear();
	return true;
}

bool Testing_class::test_2(int p) {
	if (p == 1) {
		creation_function(4);
	}
	else {
		if (p == 2) {
			cout << "--[Test on extraction]--" << endl;
		}
		creation_function(p);
	}

	cout << "Archive " << test_vector[0] << endl;
	
	bool val = extraction_function(p);
	
	if (val == false) {
		return false;
	}

	cout << size_after << "|" << size_before << endl;
	if (size_after != size_before) {
		return false;
	}

	test_vector.clear();
	return true;
}
bool Testing_class::sub_test_3() {
	cout << "User Test" << endl;
	creation_function(0);
	cout << test_vector.back();
	if (exists(test_vector[0].c_str()) == false) {
		return false;
	}

	bool val = extraction_function(0);

	if (val == false) {
		return false;
	}

	cout << size_after << "|" << size_before << endl;
	if (size_after!=size_before){
		return false;
	}

	test_vector.clear();
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
	cout << endl << "--[TestQueue without ThreadPool]--" << endl;
	try {
		function_block(queue);
		queue.simple_run();
		return true;
	}
	catch (const std::exception &e) {
		return false;
	}
}

bool Testing_class::test_8(int w) {
	bool v = false;
	cout << endl << "--[ ThreadPool without TaskQueue ]-- " << endl;
	try {
		Thread_pool thread_pool(w,v);
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
	cout << endl << "--[TaskQueue+Thread_pool (simple)]--" << endl;
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

		Thread_pool thread_pool(w, queue, v);
		return true;
	}

	catch (const std::exception &e) {
		return false;
	}
}

bool Testing_class::test_10(int w,int q){
	cout << endl << "--[ TaskQueue+Thread_Pool_Archiver]--" << endl;
	try {
		task t1,t2,t3,t4,t5,t6,t7,t8;
		bool v = false;
		TaskQueue queue(q, v);

		t1.add_function(compress_own, "newarch.tar","in1.txt","tempfile1.txt","tempfile2.txt","in2.txt");
		t2.add_function(extract, "newarch.tar");
		t3.add_function(decompress_own, "newarch.tar", "tempfile1.txt", "tempfile2.txt", "res1.txt", "res2.txt");
		//t4.add_function(comp, "in1.txt", "in2.txt","res1.txt","res2.txt");

		t5.add_function(compress_own, "newarch2.tar", "in1.txt", "tempfile1.txt", "tempfile2.txt", "in2.txt");
		t6.add_function(extract, "newarch2.tar");
		t7.add_function(decompress_own, "newarch2.tar", "tempfile1.txt", "tempfile2.txt", "res1_lib.txt", "res2_lib.txt");
		//t8.add_function(comp, "in1.txt", "in2.txt", "res1_lib.txt", "res2_lib.txt");
		
		queue.add_task("Add and Compress (own)",5,t1);
		queue.add_task("Add and Compress (lib)",5,t5);
		queue.add_task("Extract (own)",4,t2);
		queue.add_task("Extract (lib)",4,t6);
		queue.add_task("Decompress_own",3,t3);
		queue.add_task("Decompress_lib",3,t7);
		//queue.add_task("Compare",2,t4);
		//queue.add_task("Compare",1,t8);
		
		Thread_pool thread_pool(w, queue, v);
		return true;
	}

	catch (const std::exception &e) {
		return false;
	}
}