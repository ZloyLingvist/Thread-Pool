#include "ownarchiver.h"

std::shared_ptr<bi_file> ownarchiver::Open_File(const char *name, const char *mode) {
	std::shared_ptr<bi_file> b_file = std::make_shared<bi_file>();
	b_file->file = fopen(name, mode);
	b_file->rack = 0;
	b_file->mask = 0x80;
	b_file->pacifier_counter = 0;
	return b_file;
}

void ownarchiver::Close_File(std::shared_ptr<bi_file> bfile, int mode) {
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

void ownarchiver::WriteBits(std::shared_ptr<bi_file> bfile, unsigned long code, int count) {
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

unsigned long ownarchiver::ReadBits(std::shared_ptr<bi_file> bfile, int bit_count) {
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
	int next_code, character, string_code;
	unsigned int index, i;
	next_code = FIRST_CODE;
	FILE *input;
	std::shared_ptr<bi_file> b_file;
	dictionary dict[dict_size];
	
	if (FILE *file = fopen(in, "r")) { fclose(file); }
	else { return -1; }

	input = fopen(in, "rb");

	b_file = Open_File(out, "wb");

	for (i = 0; i < TABLE_SIZE; i++) {
		dict[i].code_value = UNUSED;
	}

	/* считать первый символ */
	if ((string_code = getc(input)) == EOF) {
		string_code = end_of_stream;
	}

	/* пока не конец сообщени€ */
	while ((character = getc(input)) != EOF) {
		// попытка найти в словаре фразу <фраза*, символ>
		index = find_dictionary_match(string_code, character,dict);
		/* соответствие найдено*/
		if (dict[index].code_value != -1) {
			string_code = dict[index].code_value;
		}
		else {
			/* добавление в словарь */
			if (next_code <= MAX_CODE) {
				dict[index].code_value = next_code++;
				dict[index].prefix_code = string_code;
				dict[index].character = static_cast<char>(character);
			}

			/* выдача кода */
			WriteBits(b_file, static_cast<std::make_unsigned<long>::type>(string_code), BITS);
			string_code = character;
		}
	}

	/* завершение кодировани€ */
	WriteBits(b_file, static_cast<std::make_unsigned<long>::type>(string_code), BITS);
	WriteBits(b_file, static_cast<std::make_unsigned<long>::type>(end_of_stream), BITS);
	Close_File(b_file, 0);
	fclose(input);
	return 0;
}


int ownarchiver::decompress(const char* in,const char* out) {
	unsigned int next_code, new_code, old_code;
	int character;
	unsigned int count;
	std::shared_ptr<bi_file> b_file;
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
		/* обработка неожиданной ситуации */
		if (new_code >= next_code) {
			decode_stack[0] = static_cast<char>(character);
			count = decode_string(1, old_code,dict);
		}
		else {
			count = decode_string(0, new_code,dict);
		}

		character = decode_stack[count - 1];
		/* выдача раскодированной строки */
		while (count > 0) {
			putc(decode_stack[--count], output);
		}

		/* обновление словар€ */
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


unsigned int ownarchiver::find_dictionary_match(int prefix_code, int character,dictionary *dict) {
	int index;
	int offset;

	/* получение значени€ hash функции */
	index = (character << (BITS - 8)) ^ prefix_code;
	/* разрешение возможных коллизий */

	if (index == 0) {
		offset = 1;
	}
	else {
		offset = TABLE_SIZE - index;
	}

	while (true) {
		/* €чейка словар€ не использована */
		if (dict[index].code_value == UNUSED) {
			return index;
		}

		if (dict[index].prefix_code == prefix_code && dict[index].character == static_cast<char>(character)) {
			return index;
		}

		index -= offset;
		if (index < 0) {
			index += TABLE_SIZE;
		}
	}
}

unsigned int ownarchiver::decode_string(unsigned int count, unsigned int code, dictionary *dict) {
	/* пока не встретитсь€ код символа */
	while (code > 255){
		decode_stack[count++] = dict[code].character;
		code = dict[code].prefix_code;
	}

	decode_stack[count++] = static_cast<char>(code);
	return count;
}
