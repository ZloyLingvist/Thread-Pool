#pragma once
#include "tar.h"

Tar::Tar(const char *filename,int mode) : _finished(false) {
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

Tar::~Tar() {
	if (!_finished) {
		throw("Tar file was not finished.");
	}
}

void Tar::_init(void* header) {
	std::memset(header, 0, sizeof(tarheader));
	std::sprintf(static_cast<tarheader*>(header)->indicator, "ustar  ");
	std::sprintf(static_cast<tarheader*>(header)->modiftime, "%011lo", time(NULL));
	std::sprintf(static_cast<tarheader*>(header)->filemode, "%07o", 0777);
	std::sprintf(static_cast<tarheader*>(header)->groupid, "%s", "users");
}

void Tar::_checksum(void* header) {
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

void Tar::_size(void* header, unsigned long fileSize) {
	std::sprintf(static_cast<tarheader*>(header)->filesize, "%011llo", (long long unsigned int)fileSize);
}

void Tar::_filename(void* header, const char* filename) {
	if (filename == NULL || filename[0] == 0 || std::strlen(filename) >= 100) {
		throw(20);
	}
	sprintf(static_cast<tarheader*>(header)->filename, "%s", filename); // max 100 chars !!!
}

void Tar::_endRecord(std::size_t len) {
	char c = '\0';
	while ((len % sizeof(tarheader)) != 0) {
		std::fwrite(&c, sizeof(char), sizeof(char), out);
		++len;
	}
}

void Tar::close() {
	if (!_finished) {
		_finished = true;
		tarheader header;
		std::memset((void*)&header, 0, sizeof(tarheader));
		std::fwrite((const char*)&header, sizeof(char), sizeof(tarheader), out);
		std::fwrite((const char*)&header, sizeof(char), sizeof(tarheader), out);
	}
	if (this->_closeFile) std::fclose(this->out);
}


void Tar::add_to_archive(const char* filename) {
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

long int Tar::fileLength(std::FILE *file) {
	std::fseek(file, 0L, SEEK_END);
	long int len = std::ftell(file);
	std::fseek(file, 0L, SEEK_SET);
	return len;
}

int Tar::parseoct(const char *p, size_t n){
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

int Tar::is_end_of_archive(const char *p) {
	int n;
	for (n = 511; n >= 0; --n)
		if (p[n] != '\0')
			return (0);
	return 1;
}


FILE *Tar::create_file(char *name) {
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

int Tar::verify_checksum(const char *p) {
	int n, u = 0;
	for (n = 0; n < 512; ++n) {
		if (n < 148 || n > 155)
			u += ((unsigned char *)p)[n];
		else
			u += 0x20;

	}
	return (u == parseoct(p + 148, 8));
}


void Tar::untar(FILE *a, const char *name) {
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

int Tar::extract(const char *tarFileName) {
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



