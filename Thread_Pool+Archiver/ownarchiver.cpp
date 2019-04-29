#include "ownarchiver.h"

#ifdef _WIN32
#pragma warning(disable:4996)
#endif

using namespace std;
using namespace my;

shared_ptr<bi_file> ownarchiver::Open_File(const char *name, const char *mode) {
	shared_ptr<bi_file> b_file = make_shared<bi_file>();
	b_file->file = fopen(name, mode);
	b_file->rack = 0;
	b_file->mask = 0x80;
	b_file->pacifier_counter = 0;
	return b_file;
}

void ownarchiver::Close_File(shared_ptr<bi_file> bfile, int mode) {
	if (mode == 0) {
		if (bfile->mask != 0x80) {
			putc(bfile->rack, bfile->file);
		}
		fclose(bfile->file);
	}

	if (mode == 1) {
		fclose(bfile->file);
	}
}

void ownarchiver::WriteBits(shared_ptr<bi_file> bfile, size_t code, int count) {
	unsigned long mask;
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

unsigned long ownarchiver::ReadBits(shared_ptr<bi_file> bfile, int bit_count) {
	unsigned long mask;
	unsigned long return_value;
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

int ownarchiver::compress(const char *in,const char *out) {
    size_t next_code, character, string_code;
	next_code = FIRST_CODE;
	FILE *input;
	shared_ptr<bi_file> b_file;
	dictionary dict[dict_size];
	
	if (FILE *file = fopen(in, "r")) { fclose(file); }
	else { return -1; }

	input = fopen(in, "rb");

	b_file = Open_File(out, "wb");

	for (size_t i = 0; i < TABLE_SIZE; i++) {
		dict[i].code_value = UNUSED;
	}

	if ((string_code = getc(input)) == EOF) {
		string_code = end_of_stream;
	}

	while ((character = getc(input)) != EOF) {
        size_t index = find_dictionary_match(string_code, character,dict);
		if (dict[index].code_value != -1) {
			string_code = dict[index].code_value;
		}
		else {
			if (next_code <= MAX_CODE) {
				dict[index].code_value = next_code++;
				dict[index].prefix_code = string_code;
				dict[index].character = static_cast<char>(character);
			}

			WriteBits(b_file, static_cast<make_unsigned<long>::type>(string_code), BITS);
			string_code = character;
		}
	}

	WriteBits(b_file, static_cast<make_unsigned<long>::type>(string_code), BITS);
	WriteBits(b_file, static_cast<make_unsigned<long>::type>(end_of_stream), BITS);
	Close_File(b_file, 0);
	fclose(input);
	return 0;
}


int ownarchiver::decompress(const char* in,const char* out) {
	unsigned int next_code, new_code, old_code;
	int character;
	unsigned int count;
	shared_ptr<bi_file> b_file;
	dictionary dict[dict_size];

	FILE *output;
	if (FILE *file = fopen(in, "r")){ fclose(file); }
	else { return -1; }

	b_file = Open_File(in, "rb");
	output = fopen(out, "wb");

	next_code = FIRST_CODE;
	old_code = static_cast<unsigned int>(ReadBits(b_file, BITS));

	if (old_code == end_of_stream) {
		return 1;
	}

	character = old_code;
	putc(old_code, output);

	while ((new_code = static_cast<unsigned int>(ReadBits(b_file, BITS))) != end_of_stream) {
		if (new_code >= next_code) {
			decode_stack[0] = static_cast<char>(character);
			count = decode_string(1, old_code,dict);
		}
		else {
			count = decode_string(0, new_code,dict);
		}

		character = decode_stack[count - 1];

		while (count > 0) {
			putc(decode_stack[--count], output);
		}

		if (next_code <= MAX_CODE) {
			dict[next_code].prefix_code = old_code;
			dict[next_code].character = static_cast<char>(character);
			next_code++;
		}

		old_code = new_code;
	}

	count = 0;
	Close_File(b_file, 1);
	fclose(output);
	return 0;
}


size_t ownarchiver::find_dictionary_match(size_t prefix_code, size_t character, dictionary *dict) {
    size_t index = (character << (BITS - 8)) ^ prefix_code; // x64 !! check it
    size_t offset = (index == 0) ? 1 : (TABLE_SIZE - index);

	while (true)
    {
		if (dict[index].code_value == UNUSED) // x64 access violation reading location
			return index;

		if (dict[index].prefix_code == prefix_code && dict[index].character == static_cast<char>(character))
			return index;

		index -= offset;
		if (index < 0)
			index += TABLE_SIZE;
	}
}

unsigned int ownarchiver::decode_string(unsigned int count, size_t code, dictionary *dict) {
	while (code > 255) 
	{
		decode_stack[count++] = dict[code].character;
		code = dict[code].prefix_code;
	}

	decode_stack[count++] = static_cast<char>(code);
	return count;
}
