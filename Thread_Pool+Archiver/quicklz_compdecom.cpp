#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
//http://www.quicklz.com/download.html

#include "quicklz.h"

void quick_compress(const char *in,const char *out){
	FILE *ifile, *ofile;
	char *src, *dst;
	qlz_state_compress *state_compress = (qlz_state_compress *)malloc(sizeof(qlz_state_compress));
	size_t len, len2;
	ifile = fopen(in, "rb");
	ofile = fopen(out, "wb");

	// allocate source buffer and read file
	fseek(ifile, 0, SEEK_END);
	len = ftell(ifile);
	fseek(ifile, 0, SEEK_SET);
	src = (char*)malloc(len);
	fread(src, 1, len, ifile);

	// allocate "uncompressed size" + 400 for the destination buffer
	dst = (char*)malloc(len + 400);

	// compress and write result
	len2 = qlz_compress(src, dst, len, state_compress);
	fwrite(dst, len2, 1, ofile);
	fclose(ifile);
	fclose(ofile);
}


void quick_decompress(const char *in, const char *out) {
	FILE *ifile, *ofile;
	char *src, *dst;
	qlz_state_decompress *state_decompress = (qlz_state_decompress *)malloc(sizeof(qlz_state_decompress));
    size_t len;

	ifile = fopen(in, "rb");
	ofile = fopen(out, "wb");

	// allocate source buffer
	fseek(ifile, 0, SEEK_END);
	len = ftell(ifile);
	fseek(ifile, 0, SEEK_SET);
	src = (char*)malloc(len);

	// read file and allocate destination buffer
	fread(src, 1, len, ifile);
	len = qlz_size_decompressed(src);
	dst = (char*)malloc(len);

	// decompress and write result
	len = qlz_decompress(src, dst, state_decompress);
	fwrite(dst, len, 1, ofile);
	fclose(ifile);
	fclose(ofile);
}

//int main(){
//	return 0;
//}