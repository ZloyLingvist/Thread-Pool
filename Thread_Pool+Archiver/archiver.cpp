#pragma once
#include "archive.h"

struct dictionary {
	int code_value;
	int prefix_code;
	char character;
} dict[5021];


std::shared_ptr<bi_file> LZW_archiver::Open_File(const char *name, const char *mode) {
	std::shared_ptr<bi_file> b_file = std::make_shared<bi_file>();
	b_file->file = fopen(name, mode);
	b_file->rack = 0;
	b_file->mask = 0x80;
	b_file->pacifier_counter = 0;
	return b_file;
}

void LZW_archiver::Close_File(std::shared_ptr<bi_file> bfile, int mode){
	if (mode == 0) {
		if (bfile->mask != 0x80) {
			putc(bfile->rack, bfile->file);
		}
		std::fclose(bfile->file);
	}

	if (mode == 1) {
		std::fclose(bfile->file);
	}
}

void LZW_archiver::WriteBits(std::shared_ptr<bi_file> bfile, ulong code, int count){
	ulong mask;
	mask = 1L << (count - 1);
	while (mask != 0) {
		if (mask & code) {
			bfile->rack |= bfile->mask;
		}

		bfile->mask >>= 1;
		if (bfile->mask == 0) {
			putc(bfile->rack, bfile->file);
			bfile->rack = 0;
			bfile->mask = 0x80;
		}
		mask >>= 1;
	}
}

ulong LZW_archiver::ReadBits(std::shared_ptr<bi_file> bfile, int bit_count){
	ulong mask;
	ulong return_value;
	mask = 1L << (bit_count - 1);
	return_value = 0;
	while (mask != 0) {
		if (bfile->mask == 0x80) {
			bfile->rack = getc(bfile->file);
			if (bfile->rack == EOF) {
				throw("Error while readbits!\n");
			}	
		}

		if (bfile->rack & bfile->mask) {
			return_value |= mask;
		}

		mask >>= 1;
		bfile->mask >>= 1;

		if (bfile->mask == 0) {
			bfile->mask = 0x80;
		}
	}

	return return_value;
}

int LZW_archiver::compress(FILE *input, std::shared_ptr<bi_file> bfile){
	int next_code, character, string_code;
	uint index, i;
	next_code = FIRST_CODE;

	for (i = 0; i < TABLE_SIZE; i++) {
		dict[i].code_value = UNUSED;
	}

	/* ???? ??????? */
	if ((string_code = getc(input)) == EOF) {
		string_code = end_of_stream;
	}

	/* ?? ? ??? ????? */
	while ((character = getc(input)) != EOF) {
		/* ?????????????? <??? ???> */
		index = find_dictionary_match(string_code, character);
		if (dict[index].code_value != -1) {
			string_code = dict[index].code_value;
		}
		else {
			/* ????? ????? */
			if (next_code <= MAX_CODE) {
				dict[index].code_value = next_code++;
				dict[index].prefix_code = string_code;
				dict[index].character = (char)character;
			}

			WriteBits(bfile, (ulong)string_code, BITS);
			string_code = character;
		}
	}

	/* ????? ?????? */
	WriteBits(bfile, (ulong)string_code, BITS);
	WriteBits(bfile, (ulong)end_of_stream, BITS);
	return 0;
}


int LZW_archiver::decompress(FILE *output, std::shared_ptr<bi_file> bfile) {
	uint next_code, new_code, old_code;
	int character;
	uint count;

	next_code = FIRST_CODE;
	old_code = (uint)ReadBits(bfile, BITS);

	if (old_code == end_of_stream) {
		return 1;
	}

	character = old_code;
	putc(old_code, output);

	while ((new_code = (uint)ReadBits(bfile, BITS)) != end_of_stream) {
		if (new_code >= next_code) {
			decode_stack[0] = (char)character;
			count = decode_string(1, old_code);
		}
		else {
			count = decode_string(0, new_code);
		}

		character = decode_stack[count - 1];

		while (count > 0) {
			putc(decode_stack[--count], output);
		}

		if (next_code <= MAX_CODE) {
			dict[next_code].prefix_code = old_code;
			dict[next_code].character = (char)character;
			next_code++;
		}

		old_code = new_code;
	}

	return 0;
}

//???????? ???????????? <?????,???>.?? ????? ??? ?????? ??, ??????
// ? ?????.//

uint LZW_archiver::find_dictionary_match(int prefix_code, int character){
	int index;
	int offset;

	index = (character << (BITS - 8)) ^ prefix_code;

	/* ????? ???? */
	if (index == 0) {
		offset = 1;
	}
	else {
		offset = TABLE_SIZE - index;
	}

	while (true) {
		if (dict[index].code_value == UNUSED){
			return index;
		}

		if (dict[index].prefix_code == prefix_code && dict[index].character == (char)character){
			return index;
		}

		index -= offset;
		if (index < 0) {
			index += TABLE_SIZE;
		}
	}
}

uint LZW_archiver::decode_string(uint count, uint code) {
	while (code > 255) /* ?? ? ????? ??????*/
	{
		decode_stack[count++] = dict[code].character;
		code = dict[code].prefix_code;
	}

	decode_stack[count++] = (char)code;
	return count;
}

void LZW_archiver::intf(const char *in,const char *out,int mode){
	std::shared_ptr<bi_fil? b_file;

	if (mode == 1){
		FILE *input;
		input = fopen(in, "rb");

		if (input == NULL){
			throw("Cannot read in");
		}

		b_file = Open_File(out, "wb");
		compress(input, b_file);
		Close_File(b_file, 0);
		fclose(input);
	}

	if (mode == 2){
		FILE *output;
		b_file = Open_File(in, "rb");
		output = fopen(out, "wb");
		decompress(output, b_file);
		Close_File(b_file, 1);
		fclose(output);
	}

	if (mode == 3) {
		FILE *infile, *outfile;
		infile = fopen(in, "r");
		outfile = fopen(out, "w");

		int ret = compress_zlib(infile, outfile, Z_DEFAULT_COMPRESSION);
		fclose(infile);
		fclose(outfile);
	}

	if (mode == 4) {
		FILE *infile, *outfile;
		infile = fopen(in, "r");
		outfile = fopen(out, "w");

		int ret = decompess_zlib(infile, outfile);
		fclose(infile);
		fclose(outfile);
	}
}

// ???????????
int LZW_archiver::compress_zlib(FILE *src, FILE *dst, int level){
	uint8_t inbuff[CHUNK];
	uint8_t outbuff[CHUNK];
	z_stream stream = { 0 };

	if (deflateInit(&stream, 9) != Z_OK)
	{
		fprintf(stderr, "deflateInit(...) failed!\n");
		return false;
	}

	int flush;
	do {
		stream.avail_in = fread(inbuff, 1, CHUNK, src);
		if (ferror(src))
		{
			cout << "fread(...) failed!" << endl;
			deflateEnd(&stream);
			return false;
		}

		flush = feof(src) ? Z_FINISH : Z_NO_FLUSH;
		stream.next_in = inbuff;

		do {
			stream.avail_out = CHUNK;
			stream.next_out = outbuff;
			deflate(&stream, flush);
			uint32_t nbytes = CHUNK - stream.avail_out;

			if (fwrite(outbuff, 1, nbytes, dst) != nbytes || ferror(dst))
			{
				fprintf(stderr, "fwrite(...) failed!\n");
				deflateEnd(&stream);
				return 0;
			}
		} while (stream.avail_out == 0);
	} while (flush != Z_FINISH);

	deflateEnd(&stream);
	return 1;
}

int LZW_archiver::decompess_zlib(FILE *src, FILE *dst){
	uint8_t inbuff[CHUNK];
	uint8_t outbuff[CHUNK];
	z_stream stream = { 0 };

	int result = inflateInit(&stream);
	if (result != Z_OK)
	{
		cout << "inflateInit(...) failed!" << endl;
		return false;
	}

	do {
		stream.avail_in = fread(inbuff, 1, CHUNK, src);
		if (ferror(src)) {
			cout << "fread(...) failed!" << endl;
			inflateEnd(&stream);
			return false;
		}

		if (stream.avail_in == 0)
			break;

		stream.next_in = inbuff;

		do {
			stream.avail_out = CHUNK;
			stream.next_out = outbuff;
			result = inflate(&stream, Z_NO_FLUSH);
			if (result == Z_NEED_DICT || result == Z_DATA_ERROR ||
				result == Z_MEM_ERROR)
			{
				cout << result << endl;;
				inflateEnd(&stream);
				return false;
			}

			uint32_t nbytes = CHUNK - stream.avail_out;

			if (fwrite(outbuff, 1, nbytes, dst) != nbytes || ferror(dst))
			{
				cout << "fwrite(...) failed!" << endl;
				inflateEnd(&stream);
				return false;
			}
		} while (stream.avail_out == 0);
	} while (result != Z_STREAM_END);

	inflateEnd(&stream);
	return result == Z_STREAM_END;
}
