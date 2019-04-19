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

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int  uint;

struct bi_filå {
	FILE *file;
	uchar mask;
	int rack;
	int pacifier_counter;
};

class LZW_archiver:public Tar {
protected:
	static constexpr size_t BITS = 12;
	static constexpr size_t MAX_CODE = ((1 << BITS) - 1);
	static constexpr size_t TABLE_SIZE = 5021;
	static constexpr size_t end_of_stream = 256;
	static constexpr size_t FIRST_CODE = 257;
	static constexpr size_t UNUSED = -1;
	static constexpr size_t PACIFIER_COUNT = 2047;
	static constexpr size_t CHUNK = 4096;

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
	char decode_stack[TABLE_SIZE];

	LZW_archiver(const char *filename, int mode);
	virtual ~LZW_archiver();

	void close();

	int compress(const char* in, const char* out) override;
	int decompress(const char* in, const char* out) override;

	void add_to_archive(const char* filename);
	void untar(FILE *a, const char *name);
	int extract(const char *tarFileName);
	int verify_checksum(const char *p);
	FILE *create_file(char *name);
	int is_end_of_archive(const char *p);
	int parseoct(const char *p, size_t n);

	/* ïîêà 2 äîï ôóíêöèè äëÿ áèáëèîòåêè */
	//int compress_zlib(FILE *source, FILE *dest, int level);
	//int decompess_zlib(FILE *source, FILE *dest);

	std::shared_ptr<bi_filå> Open_File(const char *name, const char *mode);
	void Close_File(std::shared_ptr<bi_filå> b, int mode);

	void WriteBits(std::shared_ptr<bi_filå> bfile, ulong code, int count);
	ulong ReadBits(std::shared_ptr<bi_filå> bfile, int bit_count);

	uint find_dictionary_match(int prefix_code, int character);
	uint decode_string(uint count, uint code);

};


