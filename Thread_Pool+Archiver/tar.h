#define _CRT_SECURE_NO_WARNINGS
#define TAR_BUFFER_SIZE 5120

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

//Описание формата https://en.wikipedia.org/wiki/Tar_(computing)
//----------- Tar --------------------
//Field offset 	Field size 	Field
//0 	100 	File name
//100 	8 	File mode
//108 	8 	Owner's numeric user ID
//116 	8 	Group's numeric user ID
//124 	12 	File size in bytes(octal base)
//136 	12 	Last modification time in numeric Unix time format(octal)
//148 	8 	Checksum for header record
//156 	1 	Link indicator(file type)
//157 	100 	Name of linked file

//--------USTAR--------
//0 	156 	(several fields, same as in old format)
//156 	1 	Type flag
//157 	100 	(same field as in old format)
//257 	6 	UStar indicator "ustar" then NUL
//263 	2 	UStar version "00"
//265 	32 	Owner user name
//297 	32 	Owner group name
//329 	8 	Device major number
//337 	8 	Device minor number
//345 	155 	Filename prefix

struct tarheader {
	char filename[100];
	char filemode[8];
	char userid[8];
	char groupid[8];
	char filesize[12];
	char modiftime[12];
	char checksum[8];
	char filetype[1];
	char linkname[100];

	char indicator[6];
	char version[2];
	char username[32];
	char groupname[32];
	char majornumber[8];
	char minornumber[8];
	char prefix[155];
	char pad[12];
};

class Tar {
protected:
	std::FILE* out;
	bool _finished;
	bool _closeFile;
	void _init(void* header);
	void _checksum(void* header);
	void _size(void* header, unsigned long fileSize);
	void _filename(void* header, const char* filename);
	void _endRecord(std::size_t len);
	long int fileLength(std::FILE *file);
private:
	ifstream inputStream;


public:
	Tar(const char *filename,int mode);
	virtual ~Tar();
	void close();
	void add_to_empty(const char* filename, const char* content);
	void add_to_archive(const char* filename, const char* nameInArchive);

	void untar(FILE *a, const char *name);
	int extract(char *tarFileName);
	int verify_checksum(const char *p);
	FILE *create_file(char *name);
	int is_end_of_archive(const char *p);
	int parseoct(const char *p, size_t n);
};

