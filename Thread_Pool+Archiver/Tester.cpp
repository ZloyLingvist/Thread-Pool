#include "tester.h"
#include "TaskQueue.h"
#include "Thread_pool.h"

bool Testing_class::test_1(int p){
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
		creation_function(p);
	}

	cout << "Archive " << test_vector[0] << endl;
	cout << "Test on extraction" << endl;
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
bool Testing_class::test_4(int w, int q) {
	try {
		task t1, t2, t3;
		bool v = false;
		t1.add_function(write, "T1\n", 2);
		t2.add_function(write, "T2\n", 3);
		t3.add_function(write, "T3\n", 1);

		TaskQueue queue(q,v);
		queue.add_task("T1", 1,t1);
		queue.add_task("T2", 3,t3);
		queue.add_task("T3", 2,t2);

		//std::thread::id main_thread_id = std::this_thread::get_id();
		//queue.run(main_thread_id);
		Thread_pool thread_pool(w, queue,v);
		return true;
	}

	catch (const std::exception &e) {
		return false;
	}
}
bool Testing_class::test_5(const char* in, const char* out) {
	//lib_archiver obj;
	//obj.compress(in,"tmp_lib.txt");
	//obj.decompress("tmp_lib.txt",out);
	//bool v = isFilesEqual(in, out);
	return true;
}

///// Для общего теста ///

void extract(int i, const char *name) {
	LZW_archiver myar(name, 1);
	myar.extract(name);
}

void compress_own(int i, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2) {
	LZW_archiver myar(name, 0);
	own_archiver o_ar;
	o_ar.compress(file1, file_tmp1);
	o_ar.compress(file2, file_tmp2);

	myar.add_to_archive(file_tmp1);
	myar.add_to_archive(file_tmp2);
	myar.close();
	return;
}

void compress_lib(int i, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2) {
	LZW_archiver myar(name, 0);
	lib_archiver l_ar;
	l_ar.compress(file1, file_tmp1);
	l_ar.compress(file2, file_tmp2);

	myar.add_to_archive(file_tmp1);
	myar.add_to_archive(file_tmp2);
	myar.close();
	return;
}

void decompress_own(int i, const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2) {
	own_archiver o_ar;
	o_ar.decompress(file_in1, file_out1);
	o_ar.decompress(file_in2, file_out2);
	return;
}

void decompress_lib(int i, const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2) {
	lib_archiver l_ar;
	l_ar.decompress(file_in1, file_out1);
	l_ar.decompress(file_in2, file_out2);
	return;
}

void comp(int i,const char *file1, const char *file2, const char *file_res1, const char *file_res2) {
	Testing_class obj;
	if (obj.isFilesEqual(file1, file_res1) == true) {
		cout << file1 << "~" << file_res1 << endl;
	} else {
		cout << file1 << "not ~" << file_res1 << endl;
	}
}

void removing(const char *name) {
	remove(name);
	remove("resfile1.txt");
	remove("resfile2.txt");
	remove("tmpfile1.txt");
	remove("tmpfile2.txt");
}

bool Testing_class::test_7(int q) {
	task t1, t2, t3, t4, t5, t6, t7, t8;
	bool v = false;
	TaskQueue queue(q, v);
	try {
		t1.add_function(compress_own, "newarch.tar", "in1.txt", "tempfile1.txt", "tempfile2.txt", "in2.txt");
		t2.add_function(extract, "newarch.tar");
		t3.add_function(decompress_own, "newarch.tar", "tempfile1.txt", "tempfile2.txt", "res1.txt", "res2.txt");
		t4.add_function(comp, "in1.txt", "in2.txt","res1.txt","res2.txt");

		t5.add_function(compress_own, "newarch2.tar", "in1.txt", "tempfile1.txt", "tempfile2.txt", "in2.txt");
		t6.add_function(extract, "newarch2.tar");
		t7.add_function(decompress_own, "newarch2.tar", "tempfile1.txt", "tempfile2.txt", "res1_lib.txt", "res2_lib.txt");
		t8.add_function(comp, "in1.txt", "in2.txt", "res1_lib.txt", "res2_lib.txt");

		queue.add_task("Add and Compress (own)", 5, t1);
		queue.add_task("Add and Compress (lib)", 5, t5);
		queue.add_task("Extract (own)", 4, t2);
		queue.add_task("Extract (lib)", 4, t6);
		queue.add_task("Decompress_own", 3, t3);
		queue.add_task("Decompress_lib", 3, t7);
		queue.simple_run();
		return true;
	}
	catch (const std::exception &e) {
		return false;
	}
}




















bool Testing_class::test_gl(int w,int q) {
	//removing("newarch.tar");

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