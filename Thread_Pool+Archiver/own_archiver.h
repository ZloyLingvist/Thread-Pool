#pragma once
#include "tar.h"
#include <memory>

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int  uint;

struct bi_filå {
	FILE *file;
	uchar mask;
	int rack;
	int pacifier_counter;
};

class own_archiver :public Tar {
protected:
	static constexpr size_t BITS = 12;
	static constexpr size_t MAX_CODE = ((1 << BITS) - 1);
	static constexpr size_t TABLE_SIZE = 5021;
	static constexpr size_t end_of_stream = 256;
	static constexpr size_t FIRST_CODE = 257;
	static constexpr size_t UNUSED = -1;
	static constexpr size_t PACIFIER_COUNT = 2047;
	static constexpr size_t CHUNK = 4096;
public:
	char decode_stack[TABLE_SIZE];
	own_archiver() = default;
	~own_archiver() = default;
	int compress(const char* in, const char* out) override;
	int decompress(const char* in, const char* out) override;

	std::shared_ptr<bi_filå> Open_File(const char *name, const char *mode);
	void Close_File(std::shared_ptr<bi_filå> b, int mode);

	void WriteBits(std::shared_ptr<bi_filå> bfile, ulong code, int count);
	ulong ReadBits(std::shared_ptr<bi_filå> bfile, int bit_count);

	uint find_dictionary_match(int prefix_code, int character);
	uint decode_string(uint count, uint code);
};