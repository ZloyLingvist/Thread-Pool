#include "libarch.h"

int lib_archiver::compress(const char* in, const char* out) {
	quick_compress(in,out);
	return 0;
}

int lib_archiver::decompress(const char* in, const char* out) {
	quick_decompress(in, out);
	return 0;
}

