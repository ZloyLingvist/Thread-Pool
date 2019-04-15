#pragma once
#include "tar.h"
#include <iostream>
#include <fstream>
#include <string>
#include  <memory>

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


class LZW_archiver {
protected:
	static constexpr size_t BITS = 12;
	static constexpr size_t MAX_CODE = ((1 << BITS) - 1);
	static constexpr size_t TABLE_SIZE = 5021;
	static constexpr size_t end_of_stream = 256;
	static constexpr size_t FIRST_CODE = 257;
	static constexpr size_t UNUSED = -1;
	static constexpr size_t PACIFIER_COUNT = 2047;

public:
	char decode_stack[TABLE_SIZE];

	LZW_archiver() = default;
	~LZW_archiver() = default;

	int compress(FILE *input, std::shared_ptr<bi_filå> bfile);
	int decompress(FILE *output, std::shared_ptr<bi_filå> bfile);

	std::shared_ptr<bi_filå> Open_File(char *name, const char *mode);
	void Close_File(std::shared_ptr<bi_filå> b, int mode);

	void WriteBits(std::shared_ptr<bi_filå> bfile, ulong code, int count);
	ulong ReadBits(std::shared_ptr<bi_filå> bfile, int bit_count);

	uint find_dictionary_match(int prefix_code, int character);
	uint decode_string(uint count, uint code);

	void intf(char *in, char *out, int mode);
};


