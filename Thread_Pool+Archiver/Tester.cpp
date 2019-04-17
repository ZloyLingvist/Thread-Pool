#include "tester.h"

string creativer() {
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
bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath) {
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
bool exists(const char *name) {
	if (FILE *file = fopen(name, "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}
double filesize(const char* filename){
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}
void create_file(vector<string> &filenames){
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
	archname = creativer()+".tar";
	filenames.push_back(archname.c_str());//первый элемент вектора имя архива
	file_amount = 1;
	filenames.resize(file_amount);

	for (i = 0; i < file_amount; i++){
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

bool test_basic(int count){
	vector<string> test_vector;
	LZW_archiver obj;
	double size_after=0,size_before = 0;
	create_file(test_vector);
	
	if (count == 1){
		cout << "Test on creation and extraction" << endl;
	}

	if (count == 2) {
		cout << "Test on creation/extraction+compress(not library)" << endl;
	}

	if (count == 3) {
		cout << "Test on creation/extraction+compress(library)" << endl;
	}
	
	Tar tar(test_vector[0].c_str(),0);
	for (size_t i = 1; i < test_vector.size(); i++){
		if (test_vector[i].length() > 0){
			size_before = size_before + filesize(test_vector[i].c_str());
	
			if (count == 1){
				tar.add_to_archive(test_vector[i].c_str());
				remove(test_vector[i].c_str());
			}

			if (count == 2){
				obj.intf(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str(), 1);
				tar.add_to_archive(("temp_" + test_vector[i]).c_str());
			}

			if (count == 3) {
				obj.intf(test_vector[i].c_str(), ("temp_" + test_vector[i]).c_str(), 3);
				tar.add_to_archive(("temp_" + test_vector[i]).c_str());
			}
		}
	}

	tar.close();
	
	cout << "Archive " << test_vector[0] << endl;
	if (exists(test_vector[0].c_str()) == false) {
		return false;
	}

	cout << "Test on extraction" << endl;
	tar.extract(test_vector[0].c_str());

	for (size_t i = 1; i < test_vector.size(); i++) {
		if (test_vector[i].length() > 0) {
			if (count == 2) {
				obj.intf(("temp_" + test_vector[i]).c_str(), ("res_"+test_vector[i]).c_str(), 2);
				if (isFilesEqual(test_vector[i].c_str(), ("res_" + test_vector[i]).c_str()) != true) {
					return false;
				};
			}

			if (count == 3) {
				obj.intf(("temp_" + test_vector[i]).c_str(), ("res_" + test_vector[i]).c_str(), 4);
			}

			size_after = size_after + filesize(("res_" + test_vector[i]).c_str());
		}
	}
	
	cout << size_after << "|" << size_before << endl;
	if (size_after == size_before) { 
		return true; 
	} else {
		return false;
	}
}

void work(const char *archname, const char *in,const char *out){
	Tar tar(archname, 0);
	LZW_archiver obj;
	obj.intf(in, "temp.txt", 1);
	tar.add_to_archive("temp.txt");
	tar.close();
	tar.extract(archname);
	obj.intf("temp.txt", out,2);
}

