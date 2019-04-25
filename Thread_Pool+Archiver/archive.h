#pragma once
#include "tar.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <assert.h>
#include "zconf.h"
#include "zlib.h"

using namespace std;

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


class LZW_archiver{
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
	static constexpr size_t TAR_BUFFER_SIZE = 5120;

private:
	ifstream inputStream;

public:
	LZW_archiver(const char *filename, int mode);
	~LZW_archiver();
	void close();
	void add_to_archive(const char* filename);
	void untar(FILE *a, const char *name);
	int extract(const char *tarFileName);
	int verify_checksum(const char *p);
	FILE *create_file(char *name);
	int is_end_of_archive(const char *p);
	int parseoct(const char *p, size_t n);
};


