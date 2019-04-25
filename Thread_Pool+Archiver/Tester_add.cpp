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

		if (std::memcmp(lBuffer, rBuffer, 1024) != 0) {
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
		filenames.push_back(archname.c_str());
		file_amount = 1 + rand() % 10;
		filenames.resize(file_amount);

		for (i = 0; i < file_amount; i++) {
			str1 = creativer();
			str1 = str1 + ".txt";
			filenames.push_back(str1);
			out.open(str1);
			text_amount = 1 + rand() % 250;
			for (j = 0; j < text_amount; j++) {
				str2 = creativer();
				out << str2 << endl;
			}
			out.close();
		}
	}
	else {
		archname = test_vector[0].c_str();
		filenames.push_back(archname.c_str());
		filenames.resize(file_amount);
		file_amount = test_vector.size() - 2;
		for (size_t i = 1; i < test_vector.size() - 1; i++) {
			filenames.push_back(test_vector[i].c_str());
		}
	}
}
void Testing_class::creation_function(int p) {
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

	LZW_archiver myar(test_vector[0].c_str(), 0);
	own_archiver o_ar;
	lib_archiver l_ar;

	for (size_t i = 1; i < test_vector.size() - minus; i++) {
		if (test_vector[i].length() > 0) {
			size_before = size_before + filesize(test_vector[i].c_str());
			if ((p == 1 || p == 4) || test_vector.back() == "1") {
				myar.add_to_archive(test_vector[i].c_str());
				rename(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str());
			}

			if (p == 2 || test_vector.back() == "2") {
				//o_ar.compress(test_vector[i], ("temp_" + test_vector[i]).c_str());
				myar.add_to_archive(("temp_" + test_vector[i]).c_str());
			}

			if (p == 3 || test_vector.back() == "3") {
				//l_ar.compress(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str());
				myar.add_to_archive(("temp_" + test_vector[i]).c_str());
			}
		}
	}

	myar.close();
}
bool Testing_class::extraction_function(int p) {
	LZW_archiver lzw(test_vector[0].c_str(), 0);
	own_archiver o_ar;
	lib_archiver l_ar;

	int minus = 0;
	lzw.extract(test_vector[0].c_str());

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
				//o_ar.decompress(("temp_" + test_vector[i]).c_str(), ("res_" + test_vector[i]).c_str());
				if (isFilesEqual(test_vector[i].c_str(), ("res_" + test_vector[i]).c_str()) != true) {
					return false;
				};
				size_after = size_after + filesize(("res_" + test_vector[i]).c_str());
			}

			if (p == 3 || test_vector.back() == "3") {
				//l_ar.decompress(("temp_" + test_vector[i]).c_str(), ("res_" + test_vector[i]).c_str());
				size_after = size_after + filesize(("res_" + test_vector[i]).c_str());
			}
		}
	}
}

void write(int i, const char *in, int count) {
	for (int i = 0; i < count; i++) {
		cout << in;
	}
}