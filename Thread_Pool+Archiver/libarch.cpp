#include "libarch.h"

int lib_archiver::compress(const char* in, const char* out) {
	if (FILE *file = fopen(in, "r")){ fclose(file);}
	else { return -1; }
	quick_compress(in,out);
	return 0;
}

int lib_archiver::decompress(const char* in, const char* out) {
	if (FILE *file = fopen(in, "r")){ fclose(file); }
	else { return -1; }
	quick_decompress(in, out);
	return 0;
}

