#pragma once
#include "archive.h"


struct dictionary {
	int code_value;
	int prefix_code;
	char character;
} dict[5021];


std::shared_ptr<bi_file> LZW_archiver::Open_File(char *name, const char *mode) {
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
			if ((bfile->pacifier_counter++ & PACIFIER_COUNT) == 0) {
				putc('.', stdout);
			}
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
			if ((bfile->pacifier_counter++ & PACIFIER_COUNT) == 0) {
				putc('.', stdout);
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

	/* Ñ÷èòàòü ïåðâûé ñèìâîë */
	if ((string_code = getc(input)) == EOF) {
		string_code = end_of_stream;
	}

	/* Ïîêà íå êîíåö ñîîáùåíèÿ */
	while ((character = getc(input)) != EOF) {
		/* Ïîïûòêà íàéòè â ñëîâàðå ïàðó <ôðàçà, ñèìâîë> */
		index = find_dictionary_match(string_code, character);
		if (dict[index].code_value != -1) {
			string_code = dict[index].code_value;
		}
		else {
			/* Äîáàâëåíèå â ñëîâàðü */
			if (next_code <= MAX_CODE) {
				dict[index].code_value = next_code++;
				dict[index].prefix_code = string_code;
				dict[index].character = (char)character;
			}

			WriteBits(bfile, (ulong)string_code, BITS);
			string_code = character;
		}
	}

	/* Çàâåðøåíèå êîäèðîâàíèÿ */
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

//Ïðîöåäóðà ïîèñêà â ñëîâàðå óêàçàííîé ïàðû <êîä ôðàçû,ñèìâîë>.Äëÿ óñêîðåíèÿ ïîèñêà èñïîëüçóåòñÿ õåø, ïîëó÷àåìûé 
// èç ïàðàìåòðîâ.//

uint LZW_archiver::find_dictionary_match(int prefix_code, int character){
	int index;
	int offset;

	index = (character << (BITS - 8)) ^ prefix_code;

	/* Ðàçðåøåíèå êîëëèçèé */
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
	while (code > 255) /* Ïîêà íå âñòðåòèòñÿ êîä ñèìâîëà */
	{
		decode_stack[count++] = dict[code].character;
		code = dict[code].prefix_code;
	}

	decode_stack[count++] = (char)code;
	return count;
}

void LZW_archiver::intf(char *in,char *out,int mode){
	std::shared_ptr<bi_filå> b_file;

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

// áèáëèîòå÷íûå ôóíêöèè
int LZW_archiver::compress_zlib(FILE *source, FILE *dest, int level){
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	/* compress until end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)deflateEnd(&strm);
			return Z_ERRNO;
		}
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);
			assert(ret != Z_STREAM_ERROR);
			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)deflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
		assert(strm.avail_in == 0);

	} while (flush != Z_FINISH);
	assert(ret == Z_STREAM_END);
	(void)deflateEnd(&strm);
	return Z_OK;
}

int LZW_archiver::decompess_zlib(FILE *source, FILE *dest){
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	/* decompress until deflate stream ends or end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)inflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
