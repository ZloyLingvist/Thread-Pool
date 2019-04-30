#include "tester.h"

using namespace std;
using namespace my;
using namespace my::test;

string Testing_class::creativer() {
	string const default_chars = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
	string randomString = "";
	size_t maxlength = 15;
	size_t length = rand() % maxlength + 1;

	size_t indexesOfRandomChars[15];
	for (size_t i = 0; i < length; ++i) {
		indexesOfRandomChars[i] = rand() % default_chars.length();
	}

	for (size_t i = 0; i < length; ++i) {
		randomString += default_chars[indexesOfRandomChars[i]];
	}

	return randomString;
}
bool Testing_class::isFilesEqual(const string& lFilePath, const string& rFilePath) {
	ifstream lFile(lFilePath.c_str(), ifstream::in | ifstream::binary);
	ifstream rFile(rFilePath.c_str(), ifstream::in | ifstream::binary);

	if (!lFile.is_open() || !rFile.is_open()) {
		logging("Cannot open first or second file");
	}

	char *lBuffer = new char[1024]();
	char *rBuffer = new char[1024]();

	while (lFile.good() || rFile.good()) {
		lFile.read(lBuffer, 1024);
		rFile.read(rBuffer, 1024);

		if (memcmp(lBuffer, rBuffer, 1024) != 0) {
			delete[] lBuffer;
			delete[] rBuffer;
			return false;
		}
	}

	delete[] lBuffer;
	delete[] rBuffer;
	return true;
}
bool Testing_class::exists(const char *name) {
	if (FILE *file = fopen(name, "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}
double Testing_class::filesize(const char* filename) {
	ifstream in(filename, ifstream::ate | ifstream::binary);
	return static_cast<double>(in.tellg());
}
void Testing_class::create_file(int p, vector<string> &filenames) {
    size_t file_amount = 0;
    size_t text_amount = 0;
    size_t i = 0;
    size_t j = 0;
	double size = 0;
	string archname = "";
	string str1 = "";
	string str2 = "";
	ofstream out;

	srand(static_cast<unsigned int>(time(NULL)));
	if (p != 0) {
		archname = "files\\"+creativer() + ".tar";
		filenames.push_back(archname.c_str());//первый элемент вектора имя архива
		file_amount = 1 + rand() % 10;
		filenames.resize(file_amount);

		for (i = 0; i < file_amount; i++) {
			str1 = "files\\"+creativer();//имя файла
			str1 = str1 + ".txt";
			filenames.push_back(str1);
			out.open(str1);
			text_amount = 1 + rand() % 250;//количество случайных строк
			for (j = 0; j < text_amount; j++) {
				str2 = creativer();//строка
				out << str2 << endl;
			}
			out.close();
		}
	}
	else {
		archname = test_vector[0].c_str();
		filenames.push_back(archname.c_str());
		filenames.resize(file_amount);
		file_amount = test_vector.size() - 2;//первый и последний выкидываем
		for (size_t i = 1; i < test_vector.size() - 1; i++) {
			filenames.push_back(test_vector[i].c_str());
		}
	}
}
void Testing_class::creation_function(int p,int log) {
	int minus = 0;

	if (p == 0) {
		minus = 1;
	}
	else {
		create_file(p, test_vector);
	}

	if (p == 1) {
		logging("Test on creation");
	}

	tarchiver myar(test_vector[0].c_str(),"a",log);
	ownarchiver o_ar;
	lib_archiver l_ar;
	
	for (size_t i = 1; i < test_vector.size() - minus; i++) {
		if (test_vector[i].length() > 0) {
			size_before = size_before + filesize(test_vector[i].c_str());
			if ((p == 1 || p == 4) || test_vector.back() == "1") {
				myar.add_to_archive(test_vector[i].c_str());
				rename(test_vector[i].c_str(), ("files\\temp_" + test_vector[i]).c_str());
			}

			if (p == 2 || test_vector.back() == "2") {
				o_ar.compress(test_vector[i].c_str(), ("files\\temp_" + test_vector[i]).c_str());
				myar.add_to_archive(("files\\temp_" + test_vector[i]).c_str());
			}

			if (p == 3 || test_vector.back() == "3") {
				l_ar.compress(test_vector[i].c_str(), ("files\\temp_" + test_vector[i]).c_str());
				myar.add_to_archive(("files\\temp_" + test_vector[i]).c_str());
			}
		}
	}

	myar.close();
}
bool Testing_class::extraction_function(int p,int log) {
	tarchiver lzw(test_vector[0].c_str(),"e",log);
	ownarchiver o_ar;
	lib_archiver l_ar;

	int minus = 0;
	
	if (p == 0) {
		minus = 1;
	}

	for (size_t i = 1; i < test_vector.size() - minus; i++) {
		if (test_vector[i].length() > 0) {
			if (p == 1 || test_vector.back() == "1") {
				cout << test_vector[i].c_str() << " " << ("temp_" + test_vector[i]).c_str() << endl;
				if (isFilesEqual(test_vector[i].c_str(), ("files\\temp_" + test_vector[i]).c_str()) != true) {
					return false;
				};

				size_after = size_after + filesize(("files\\temp_" + test_vector[i]).c_str());
			}

			if (p == 2 || test_vector.back() == "2") {
				o_ar.decompress(("files\\temp_" + test_vector[i]).c_str(), ("files\\res_" + test_vector[i]).c_str());
				if (isFilesEqual(test_vector[i].c_str(), ("files\\res_" + test_vector[i]).c_str()) != true) {
					return false;
				};
				size_after = size_after + filesize(("files\\res_" + test_vector[i]).c_str());
			}

			if (p == 3 || test_vector.back() == "3") {
				l_ar.decompress(("files\\temp_" + test_vector[i]).c_str(), ("files\\res_" + test_vector[i]).c_str());
				size_after = size_after + filesize(("files\\res_" + test_vector[i]).c_str());
			}
		}
	}
    return true;
}
void Testing_class::cleaner() {
	for (size_t i = 1; i < test_vector.size(); i++) {
		remove(test_vector[i].c_str());
		remove(("files\\temp_"+test_vector[i]).c_str());
		remove(("files\\res_" + test_vector[i]).c_str());
	}
	test_vector.clear();
}

/*-------------------------------------------------------*/

void my::test::my_write(const char *in, int count) {
	for (int k = 0; k < count; k++) {
		cout << in;
	}
}
void my::test::function_block(TaskQueue &queue,int log) {
	task t1, t2, t3, t4, t5, t6, t7, t8,t9;
	string t_n1 = "T1";
	string t_n2 = "T2";
	string t_n3 = "T3";
	string t_n4 = "T4";
	string t_n5 = "T5";
	int k = 10;

	for (int i = 0; i < queue.capacity(); i++) {
		t1.add_task(t_n1, 5+k, compress_own, log, "files\\newarch.tar", "in1.txt", "files\\tempfile1.txt", "files\\tempfile2.txt", "in2.txt");
		t2.add_task(t_n2, 4+k, extract, log, "files\\newarch.tar");
		t3.add_task(t_n3, 3+k, decompress_own, "files\\newarch.tar", "files\\tempfile1.txt", "files\\tempfile2.txt", "files\\res1.txt", "files\\res2.txt");
		t4.add_task(t_n4, 2+k, comp, "in1.txt", "in2.txt", "files\\res1.txt", "files\\res2.txt",log);

		t5.add_task(t_n1, 5+k, compress_lib, log, "files\\newarch2.tar", "in1.txt", "files\\tempfile1_lib.txt", "files\\tempfile2_lib.txt", "in2.txt");
		t6.add_task(t_n2, 4+k, extract, log, "files\\newarch2.tar");
		t7.add_task(t_n3, 3+k, decompress_lib, "files\\newarch2.tar", "files\\tempfile1_lib.txt", "files\\tempfile2_lib.txt", "files\\res1_lib.txt", "res2_lib.txt");
		t8.add_task(t_n4, 2+k, comp, "in1.txt", "in2.txt", "files\\res1_lib.txt", "files\\res2_lib.txt",log);
		t9.add_task(t_n5, 6+k, iteration, i);

		queue.push(t9);
		queue.push(t1);
		queue.push(t5);
		queue.push(t2);
		queue.push(t6);
		queue.push(t3);
		queue.push(t7);
		queue.push(t4);
		queue.push(t8);
		k = k +10;
	}
}

void my::test::extract(int log, const char *name) {
	tarchiver myar(name,"e",log);
}

void my::test::compress_own(int log,const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2) {
	tarchiver myar(name,"a",log);
	ownarchiver o_ar;
	o_ar.compress(file1, file_tmp1);
	o_ar.compress(file2, file_tmp2);

	myar.add_to_archive(file_tmp1);
	myar.add_to_archive(file_tmp2);
	myar.close();
	return;
}
void my::test::compress_lib(int log, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2) {
	tarchiver myar(name,"a",log);
	lib_archiver l_ar;
	l_ar.compress(file1, file_tmp1);
	l_ar.compress(file2, file_tmp2);

	myar.add_to_archive(file_tmp1);
	myar.add_to_archive(file_tmp2);
	myar.close();
	return;
}

void my::test::decompress_own(const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2) {
	ownarchiver o_ar;
	o_ar.decompress(file_in1, file_out1);
	o_ar.decompress(file_in2, file_out2);
	return;
}
void my::test::decompress_lib(const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2) {
	lib_archiver l_ar;
	l_ar.decompress(file_in1, file_out1);
	l_ar.decompress(file_in2, file_out2);
	return;
}

void my::test::iteration(int i) {
	cout << "Iteration " << i << endl;
	vector <const char*> m = { "files\\tempfile1_lib.txt", "files\\tempfile2_lib.txt","files\\newarch.tar", "files\\tempfile1.txt", "files\\tempfile2.txt", "files\\newarch2.tar","files\\res1.txt", "files\\res2.txt", "files\\res1_lib.txt", "files\\res2_lib.txt" };
	for (size_t t= 0; t < m.size(); t++) {
		remove(m[t]);
	}
	return;
}


void my::test::void_task(int a,int b,int n, const char *text){
	cout << "Calculate" << endl;
	for (int k = 0; k < n; k++) {
		a = a*b;
	}
	cout << text << " " << a;
}
int my::test::int_task(int a, int b) {
	cout << a + b << endl;
	return (a + b);
}
string my::test::string_test(const char *str1, const char *str2) {
	cout << str1 << " " << str2 << endl;
	return str1;
}