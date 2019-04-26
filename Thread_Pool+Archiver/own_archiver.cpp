#include "own_archiver.h"

struct dictionary {
	int code_value;
	int prefix_code;
	char character;
} dict[5021];


std::shared_ptr<bi_filе> own_archiver::Open_File(const char *name, const char *mode) {
	std::shared_ptr<bi_filе> b_file = std::make_shared<bi_filе>();
	b_file->file = fopen(name, mode);
	b_file->rack = 0;
	b_file->mask = 0x80;
	b_file->pacifier_counter = 0;
	return b_file;
}

void own_archiver::Close_File(std::shared_ptr<bi_filе> bfile, int mode) {
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

void own_archiver::WriteBits(std::shared_ptr<bi_filе> bfile, ulong code, int count) {
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

ulong own_archiver::ReadBits(std::shared_ptr<bi_filе> bfile, int bit_count) {
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

int own_archiver::compress(const char *in,const char *out) {
	int next_code, character, string_code;
	uint index, i;
	next_code = FIRST_CODE;
	FILE *input;
	std::shared_ptr<bi_filе> b_file;
	input = fopen(in, "rb");

	if (input == NULL) {
		throw("Cannot read in");
	}

	b_file = Open_File(out, "wb");

	for (i = 0; i < TABLE_SIZE; i++) {
		dict[i].code_value = UNUSED;
	}

	/* Считать первый символ */
	if ((string_code = getc(input)) == EOF) {
		string_code = end_of_stream;
	}

	/* Пока не конец сообщения */
	while ((character = getc(input)) != EOF) {
		/* Попытка найти в словаре пару <фраза, символ> */
		index = find_dictionary_match(string_code, character);
		if (dict[index].code_value != -1) {
			string_code = dict[index].code_value;
		}
		else {
			/* Добавление в словарь */
			if (next_code <= MAX_CODE) {
				dict[index].code_value = next_code++;
				dict[index].prefix_code = string_code;
				dict[index].character = (char)character;
			}

			WriteBits(b_file, (ulong)string_code, BITS);
			string_code = character;
		}
	}

	/* Завершение кодирования */
	WriteBits(b_file, (ulong)string_code, BITS);
	WriteBits(b_file, (ulong)end_of_stream, BITS);
	Close_File(b_file, 0);
	fclose(input);
	return 0;
}


int own_archiver::decompress(const char* in,const char* out) {
	uint next_code, new_code, old_code;
	int character;
	uint count;
	std::shared_ptr<bi_filе> b_file;

	FILE *output;
	b_file = Open_File(in, "rb");
	output = fopen(out, "wb");

	next_code = FIRST_CODE;
	old_code = (uint)ReadBits(b_file, BITS);

	if (old_code == end_of_stream) {
		return 1;
	}

	character = old_code;
	putc(old_code, output);

	while ((new_code = (uint)ReadBits(b_file, BITS)) != end_of_stream) {
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

	Close_File(b_file, 1);
	fclose(output);
	return 0;
}

//Процедура поиска в словаре указанной пары <код фразы,символ>.Для ускорения поиска используется хеш, получаемый 
// из параметров.//

uint own_archiver::find_dictionary_match(int prefix_code, int character) {
	int index;
	int offset;

	index = (character << (BITS - 8)) ^ prefix_code;

	/* Разрешение коллизий */
	if (index == 0) {
		offset = 1;
	}
	else {
		offset = TABLE_SIZE - index;
	}

	while (true) {
		if (dict[index].code_value == UNUSED) {
			return index;
		}

		if (dict[index].prefix_code == prefix_code && dict[index].character == (char)character) {
			return index;
		}

		index -= offset;
		if (index < 0) {
			index += TABLE_SIZE;
		}
	}
}

uint own_archiver::decode_string(uint count, uint code) {
	while (code > 255) /* Пока не встретится код символа */
	{
		decode_stack[count++] = dict[code].character;
		code = dict[code].prefix_code;
	}

	decode_stack[count++] = (char)code;
	return count;
}
