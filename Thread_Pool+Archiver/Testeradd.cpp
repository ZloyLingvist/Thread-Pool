#include "tester.h"

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
bool Testing_class::isFilesEqual(const std::string& lFilePath, const std::string& rFilePath) {
	std::ifstream lFile(lFilePath.c_str(), std::ifstream::in | std::ifstream::binary);
	std::ifstream rFile(rFilePath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!lFile.is_open() || !rFile.is_open()) {
		cout << "Cannot open first or second file" << endl;
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
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}
void Testing_class::create_file(int p, vector<string> &filenames) {
	int file_amount = 0;
	int text_amount = 0;
	int i = 0;
	int j = 0;
	double size = 0;
	string archname = "";
	string str1 = "";
	string str2 = "";
	ofstream out;

	srand(time(NULL));
	if (p != 0) {
		archname = creativer() + ".tar";
		cout << archname << endl;
		filenames.push_back(archname.c_str());//первый элемент вектора имя архива
		file_amount = 1 + rand() % 10;
		filenames.resize(file_amount);

		for (i = 0; i < file_amount; i++) {
			str1 = creativer();//имя файла
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
		cout << "Test on creation" << endl;
	}

	tarchiver myar(test_vector[0].c_str(),"a",log);
	ownarchiver o_ar;
	lib_archiver l_ar;
	
	for (size_t i = 1; i < test_vector.size() - minus; i++) {
		if (test_vector[i].length() > 0) {
			size_before = size_before + filesize(test_vector[i].c_str());
			if ((p == 1 || p == 4) || test_vector.back() == "1") {
				myar.add_to_archive(test_vector[i].c_str());
				rename(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str());
			}

			if (p == 2 || test_vector.back() == "2") {
				o_ar.compress(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str());
				myar.add_to_archive(("temp_" + test_vector[i]).c_str());
			}

			if (p == 3 || test_vector.back() == "3") {
				l_ar.compress(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str());
				myar.add_to_archive(("temp_" + test_vector[i]).c_str());
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
				if (isFilesEqual(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str()) != true) {
					return false;
				};

				size_after = size_after + filesize(("temp_" + test_vector[i]).c_str());
			}

			if (p == 2 || test_vector.back() == "2") {
				o_ar.decompress(("temp_" + test_vector[i]).c_str(), ("res_" + test_vector[i]).c_str());
				if (isFilesEqual(test_vector[i].c_str(), ("res_" + test_vector[i]).c_str()) != true) {
					return false;
				};
				size_after = size_after + filesize(("res_" + test_vector[i]).c_str());
			}

			if (p == 3 || test_vector.back() == "3") {
				l_ar.decompress(("temp_" + test_vector[i]).c_str(), ("res_" + test_vector[i]).c_str());
				size_after = size_after + filesize(("res_" + test_vector[i]).c_str());
			}
		}
	}
}
void Testing_class::cleaner() {
	for (size_t i = 1; i < test_vector.size(); i++) {
		if (test_vector[i] == "in1.txt" || test_vector[i] == "in2.txt") {
			continue;
		}
		remove(test_vector[i].c_str());
		remove(("temp_"+test_vector[i]).c_str());
		remove(("res_" + test_vector[i]).c_str());
	}
	test_vector.clear();
}

/*-------------------------------------------------------*/

void write(int i, const char *in, int count) {
	for (int k = 0; k < count; k++) {
		cout << in;
	}
}
void function_block(TaskQueue &queue,int log) {
	removing("newarch.tar", "tempfile1.txt", "tempfile2.txt", "newarch2.tar");
	task t1, t2, t3, t4, t5, t6, t7, t8;
	t1.add_function(compress_own,log,"newarch.tar", "in1.txt", "tempfile1.txt", "tempfile2.txt", "in2.txt");
	t2.add_function(extract,log,"newarch.tar");
	t3.add_function(decompress_own,"newarch.tar", "tempfile1.txt", "tempfile2.txt", "res1.txt", "res2.txt");
	t4.add_function(comp,"in1.txt", "in2.txt", "res1.txt", "res2.txt");

	t5.add_function(compress_own, log,"newarch2.tar", "in1.txt", "tempfile1.txt", "tempfile2.txt", "in2.txt");
	t6.add_function(extract,log,"newarch2.tar");
	t7.add_function(decompress_own,"newarch2.tar", "tempfile1.txt", "tempfile2.txt", "res1_lib.txt", "res2_lib.txt");
	t8.add_function(comp,"in1.txt", "in2.txt", "res1_lib.txt", "res2_lib.txt");

	queue.add_task("Add and Compress (own)", 5, t1);
	queue.add_task("Add and Compress (lib)", 5, t5);
	queue.add_task("Extract (own)", 4, t2);
	queue.add_task("Extract (lib)", 4, t6);
	queue.add_task("Decompress_own", 3, t3);
	queue.add_task("Decompress_lib", 3, t7);
	queue.add_task("Compare_own", 2, t4);
	queue.add_task("Compare_lib", 2, t8);
}
void removing(const char *name, const char *in1, const char *in2, const char *in3) {
	remove(name);
	remove(in1);
	remove(in2);
	remove(in3);
}
void extract(int i, int log, const char *name) {
	tarchiver myar(name,"e",log);
}

void compress_own(int i, int log,const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2) {
	tarchiver myar(name,"a",log);
	ownarchiver o_ar;
	o_ar.compress(file1, file_tmp1);
	o_ar.compress(file2, file_tmp2);

	myar.add_to_archive(file_tmp1);
	myar.add_to_archive(file_tmp2);
	myar.close();
	return;
}
void compress_lib(int i, int log, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2) {
	tarchiver myar(name,"a",log);
	lib_archiver l_ar;
	l_ar.compress(file1, file_tmp1);
	l_ar.compress(file2, file_tmp2);

	myar.add_to_archive(file_tmp1);
	myar.add_to_archive(file_tmp2);
	myar.close();
	return;
}

void decompress_own(int i, const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2) {
	ownarchiver o_ar;
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

void void_task(int i,int a,int b,int n, const char *text){
	cout << "Calculate" << endl;
	for (int k = 0; k < n; k++) {
		a = a*b;
	}
	cout << text << " " << a;
}
int int_task(int i,int a, int b) {
	cout << a + b << endl;
	return (a + b);
}
string string_test(int i, const char *str1, const char *str2) {
	cout << str1 << " " << str2 << endl;
	return str1;
}