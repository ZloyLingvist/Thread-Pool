#ifndef TARBALLLIB_H_
#define TARBALLLIB_H_

#define _CRT_SECURE_NO_WARNINGS

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#define TAR_BUFFER_SIZE 5120

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct tarheader {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

class Tar {
protected:
	bool _finished;
	bool _closeFile;

protected:
	std::FILE* out;
	void _init(void* header);
	void _checksum(void* header);
	void _size(void* header, unsigned long fileSize);
	void _filename(void* header, const char* filename);
	void _endRecord(std::size_t len);
	long int fileLength(std::FILE *file);
private:
	ifstream inputStream;
	vector<tarheader> tarVector;  /// храним tar header
	int usBlockSize;
	int fileNum;  
	int hex2Dec(const char*, int); 

public:
	Tar(const char *filename,int mode);
	virtual ~Tar();
	void close();
	void put(const char* filename, const std::string& s);
	void put(const char* filename, const char* content);
	void put(const char* filename, const char* content, std::size_t len);

	void putFile(const char* filename, const char* nameInArchive);
	void showContent(const char *file);  //содержимое архива
	int getFileNum(); /// количество файлов в архиве
	bool nextHeader(ifstream& inputFile, int nextHeaderBlock, int lengthOfFile, int position);
	tarheader readHeader(ifstream& inputFile, int nextHeaderBlock, int position);
	int convertSizeToInt(char size[12]);
	void writeBody(ifstream& inputFile, tarheader& header);
	int extract();//извлечение
};



#endif /* TARBALLLIB_H_ */