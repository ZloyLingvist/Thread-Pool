#pragma once
#include <cstdio>
#include <string>
#include "Thread_pool.h"
#include "TaskQueue.h"
#include "archive.h"
#include <time.h>

bool v = false;
int p = 1;

bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath){
	std::ifstream lFile(lFilePath.c_str(), std::ifstream::in | std::ifstream::binary);
	std::ifstream rFile(rFilePath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!lFile.is_open() || !rFile.is_open())
	{
		return false;
	}

	char *lBuffer = new char[1024]();
	char *rBuffer = new char[1024]();

	while (lFile.good() || rFile.good()) {
		lFile.read(lBuffer, 1024);
		rFile.read(rBuffer, 1024);

		if (std::memcmp(lBuffer, rBuffer, 1024) != 0)
		{
			delete[] lBuffer;
			delete[] rBuffer;
			return false;
		}
	}

	delete[] lBuffer;
	delete[] rBuffer;
	return true;
}


/*------------ Задачи ---------------*/
void archive_test(){
	ifstream inFile;
	ofstream outFile;
	LZW_archiver obj;

	char *in = "r.txt";
	char *out = "res.txt";

	/* создали архив */
	cout << "Make archive" << endl;
	Tar tar("archive.tar");
	
	cout << "Try to compress" << endl;
	obj.intf(in, "compress_1.txt", 1); //1-сжатие 2-расжатие

	tar.add_to_archive(in, "ct.txt");
	tar.add_to_archive("compress_1.txt", "p.txt");
	remove("compress_1.txt");
	tar.close();
	
	cout << "Try to extract" << endl;
	tar.extract("archive.tar");
	
	obj.intf("p.txt",out,2); //1-сжатие 2-расжатие

	cout << "Compare" << endl;
	if (isFilesEqual(in,out) == true) {
		cout << "equivalent" << endl;
	}
	else {
		cout << "not equivalent" << endl;
	}

	remove("archive.tar");
	remove("p.txt");
	remove(out);
}


int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "Russian");
	int w = 1;
	int q = 1;
	
	/*vector<string> arr = {"Архиватор в цикле"};
	vector<std::function<void()>> functions={archive_test};
	
	TaskQueue queue(q,arr,functions);
	std::thread::id main_thread_id = std::this_thread::get_id();
	queue.add_task(main_thread_id);
	
	Thread_pool thread_pool(w);
	thread_pool.init(w, queue);*/

	archive_test();
	system("pause");
	return 0;
}
