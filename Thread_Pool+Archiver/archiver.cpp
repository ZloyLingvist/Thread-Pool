#pragma once
#include "archive.h"

LZW_archiver::LZW_archiver(const char *filename, int mode) : _finished(false) {
	if (mode == 0) {
		this->out = std::fopen(filename, "wb");
		if (out == NULL) {
			throw("Cannot open output");
		}
	}

	this->_closeFile = true;
	this->_finished = true;
	if (sizeof(tarheader) != 512) {
		throw(sizeof(tarheader));
	}
}

LZW_archiver:: ~LZW_archiver() {
	if (!_finished) {
		throw("Tar file was not finished.");
	}
}

void LZW_archiver::_init(void* header) {
	std::memset(header, 0, sizeof(tarheader));
	std::sprintf(static_cast<tarheader*>(header)->indicator, "ustar  ");
	std::sprintf(static_cast<tarheader*>(header)->modiftime, "%011lo", time(NULL));
	std::sprintf(static_cast<tarheader*>(header)->filemode, "%07o", 0777);
	std::sprintf(static_cast<tarheader*>(header)->groupid, "%s", "users");
}

void LZW_archiver::_checksum(void* header) {
	unsigned int sum = 0;
	char *p = (char *)header;
	char *q = p + sizeof(tarheader);
	while (p < static_cast<tarheader*>(header)->checksum) {
		sum += *p++ & 0xff;
	}

	for (int i = 0; i < 8; ++i) {
		sum += ' ';
		++p;
	}

	while (p < q) {
		sum += *p++ & 0xff;
	}

	std::sprintf(static_cast<tarheader*>(header)->checksum, "%06o", sum);
}

void LZW_archiver::_size(void* header, unsigned long fileSize) {
	std::sprintf(static_cast<tarheader*>(header)->filesize, "%011llo", (long long unsigned int)fileSize);
}

void LZW_archiver::_filename(void* header, const char* filename) {
	if (filename == NULL || filename[0] == 0 || std::strlen(filename) >= 100) {
		throw(20);
	}
	sprintf(static_cast<tarheader*>(header)->filename, "%s", filename); // max 100 chars !!!
}

void LZW_archiver::_endRecord(std::size_t len) {
	char c = '\0';
	while ((len % sizeof(tarheader)) != 0) {
		std::fwrite(&c, sizeof(char), sizeof(char), out);
		++len;
	}
}

void LZW_archiver::close() {
	if (!_finished) {
		_finished = true;
		tarheader header;
		std::memset((void*)&header, 0, sizeof(tarheader));
		std::fwrite((const char*)&header, sizeof(char), sizeof(tarheader), out);
		std::fwrite((const char*)&header, sizeof(char), sizeof(tarheader), out);
	}
	if (this->_closeFile) std::fclose(this->out);
}


void LZW_archiver::add_to_archive(const char* filename) {
	std::FILE* in = std::fopen(filename, "rb");
	if (in == NULL) {
		throw(10);
	}

	std::fseek(in, 0L, SEEK_END);
	long int len = std::ftell(in);
	std::fseek(in, 0L, SEEK_SET);

	tarheader header;
	_init((void*)&header);
	_filename((void*)&header, filename);
	header.filetype[0] = 0;
	_size((void*)&header, len);
	_checksum((void*)&header);
	std::fwrite((const char*)&header, sizeof(char), sizeof(tarheader), out);

	char buff[BUFSIZ];
	unsigned long int total = 0;
	std::size_t nRead = 0;
	while ((nRead = std::fread(buff, sizeof(char), BUFSIZ, in)) > 0) {
		std::fwrite(buff, sizeof(char), nRead, out);
		total = total + nRead;
	}

	std::fclose(in);
	_endRecord(total);
}

long int LZW_archiver::fileLength(std::FILE *file) {
	std::fseek(file, 0L, SEEK_END);
	long int len = std::ftell(file);
	std::fseek(file, 0L, SEEK_SET);
	return len;
}

int LZW_archiver::parseoct(const char *p, size_t n) {
	int i = 0;
	while ((*p < '0' || *p > '7') && n > 0) {
		++p;
		--n;
	}

	while (*p >= '0' && *p <= '7' && n > 0) {
		i *= 8;
		i += *p - '0';
		++p;
		--n;
	}
	return (i);
}

int LZW_archiver::is_end_of_archive(const char *p) {
	int n;
	for (n = 511; n >= 0; --n)
		if (p[n] != '\0')
			return (0);
	return 1;
}


FILE *LZW_archiver::create_file(char *name) {
	FILE *f;
	f = fopen(name, "wb+");
	if (f == NULL) {
		char *p = strrchr(name, '/');
		if (p != NULL) {
			f = fopen(name, "wb+");
		}
	}
	return f;
}

int LZW_archiver::verify_checksum(const char *p) {
	int n, u = 0;
	for (n = 0; n < 512; ++n) {
		if (n < 148 || n > 155)
			u += ((unsigned char *)p)[n];
		else
			u += 0x20;

	}
	return (u == parseoct(p + 148, 8));
}


void LZW_archiver::untar(FILE *a, const char *name) {
	char buff[512];
	FILE *f = NULL;
	size_t bytes_read;
	int filesize;

	cout << "Extracting from " << name << endl;
	while (true) {
		bytes_read = fread(buff, 1, 512, a);

		if (bytes_read < 512) {
			return;
		}

		if (is_end_of_archive(buff)) {
			cout << "End of " << name << endl;
			return;
		}

		if (!verify_checksum(buff)) {
			cout << "Checksum failure" << endl;
			return;
		}

		filesize = parseoct(buff + 124, 12);
		cout << " Extracting file " << buff << endl;
		f = create_file(buff);

		while (filesize > 0) {
			bytes_read = fread(buff, 1, 512, a);
			if (bytes_read < 512) {
				return;
			}

			if (filesize < 512) {
				bytes_read = filesize;
			}

			if (f != NULL) {
				if (fwrite(buff, 1, bytes_read, f) != bytes_read) {
					cout << "Failed write " << endl;
					fclose(f);
					f = NULL;
				}
			}

			filesize -= bytes_read;
		}

		if (f != NULL) {
			fclose(f);
			f = NULL;
		}
	}
}

int LZW_archiver::extract(const char *tarFileName) {
	FILE *a;
	a = fopen(tarFileName, "rb");
	if (a == NULL) {
		cout << "Unable to open infile" << endl;
	}

	else {
		untar(a, tarFileName);
		fclose(a);
	}

	return 0;
}













































struct dictionary {
	int code_value;
	int prefix_code;
	char character;
} dict[5021];


std::shared_ptr<bi_filе> LZW_archiver::Open_File(const char *name, const char *mode) {
	std::shared_ptr<bi_filе> b_file = std::make_shared<bi_filе>();
	b_file->file = fopen(name, mode);
	b_file->rack = 0;
	b_file->mask = 0x80;
	b_file->pacifier_counter = 0;
	return b_file;
}

void LZW_archiver::Close_File(std::shared_ptr<bi_filе> bfile, int mode){
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

void LZW_archiver::WriteBits(std::shared_ptr<bi_filе> bfile, ulong code, int count){
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

ulong LZW_archiver::ReadBits(std::shared_ptr<bi_filе> bfile, int bit_count){
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

int LZW_archiver::compress(const char *in,const char *out){
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


int LZW_archiver::decompress(const char* in, const char* out) {
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
	putc(old_code,output);

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

uint LZW_archiver::find_dictionary_match(int prefix_code, int character){
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
	while (code > 255) /* Пока не встретится код символа */
	{
		decode_stack[count++] = dict[code].character;
		code = dict[code].prefix_code;
	}

	decode_stack[count++] = (char)code;
	return count;
}

