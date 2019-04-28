#pragma once
#include "archiver.h"

tarchiver::tarchiver(const char *filename, const char *mode,int md) : finished(false) {
	log = md;
	if (mode == "a"){
		string my("Archive name is : ");
		my = my + filename;
		logging(my.c_str());

		this->out = fopen(filename, "wb");
		if (out == NULL) {
			string my(filename);
			my = "Cannot open " + my;
			logging(my.c_str());
			throw(my);
		}
	}

	this->closeFile = true;
	this->finished = true;
	if (sizeof(tarheader) != 512) {
		throw(sizeof(tarheader));
	}

	if (mode == "e") {
		this->extract(filename);
	}
}

tarchiver:: ~tarchiver() {
	if (!finished) {
		logging("Tar file was not finished.");
		throw("Tar file was not finished.");
	}
}

void tarchiver::init(void* header) {
	memset(header, 0, sizeof(tarheader));
	sprintf(static_cast<tarheader*>(header)->indicator, "ustar  ");
	sprintf(static_cast<tarheader*>(header)->modiftime, "%011lo", time(NULL));
	sprintf(static_cast<tarheader*>(header)->filemode, "%07o", 0777);
	sprintf(static_cast<tarheader*>(header)->groupid, "%s", "users");
}

void tarchiver::checksum(void* header) {
	unsigned int sum = 0;
	int i = 0;
	char *p = static_cast<char*>(header);
	char *q = p + sizeof(tarheader);

	while (p < static_cast<tarheader*>(header)->checksum) {
		sum += *p++ & 0xff;
	}

	for (i = 0; i < 8; ++i){
		sum += ' ';
		++p;
	}

	while (p < q) {
		sum += *p++ & 0xff;
	}

	sprintf(static_cast<tarheader*>(header)->checksum, "%06o", sum);
}

void tarchiver::size(void* header, unsigned long fileSize) {
	sprintf(static_cast<tarheader*>(header)->filesize, "%011llo", static_cast<long long unsigned int>(fileSize));
}

void tarchiver::filename_(void* header, const char* filename) {
	if (filename == NULL || filename[0] == 0 || strlen(filename) >= 100){
		throw("Invalid file name");
	}
	sprintf(static_cast<tarheader*>(header)->filename, "%s", filename); 
}

void tarchiver::endRecord(std::size_t len) {
	char c = '\0';
	while ((len % sizeof(tarheader)) != 0){
		fwrite(&c, sizeof(char), sizeof(char), out);
		++len;
	}
}

void tarchiver::close() {
	if (!finished) {
		finished = true;
		memset(static_cast<void*>(&header), 0, sizeof(tarheader));
		fwrite(static_cast<char*>(static_cast<void*>(&header)), sizeof(char), sizeof(tarheader), out);
		fwrite(static_cast<char*>(static_cast<void*>(&header)), sizeof(char), sizeof(tarheader), out);
	}
	if (this->closeFile){
		fclose(this->out);
	}
}


void tarchiver::add_to_archive(const char* filename) {
	FILE* in = std::fopen(filename, "rb");
	char buff[BUFSIZ];
	unsigned long int total = 0;
	size_t nRead = 0;

	if (in == NULL) {
		throw(10);
	}

	fseek(in, 0L, SEEK_END);
	long int len = ftell(in);
	fseek(in, 0L, SEEK_SET);

	init(static_cast<void*>(&header));
	filename_(static_cast<void*>(&header), filename);
	header.filetype[0] = 0;
	size(static_cast<void*>(&header), len);
	checksum(static_cast<void*>(&header));
	std::fwrite(static_cast<char*>(static_cast<void*>(&header)), sizeof(char), sizeof(tarheader), out);

	while ((nRead = fread(buff, sizeof(char), BUFSIZ, in)) > 0){
		fwrite(buff, sizeof(char), nRead, out);
		total = total + nRead;
	}

	std::fclose(in);
	string my("Add to archive: ");
	my = my + filename;
	logging(my.c_str());
	endRecord(total);
}

long int tarchiver::fileLength(std::FILE *file) {
	fseek(file, 0L, SEEK_END);
	long int len = std::ftell(file);
	fseek(file, 0L, SEEK_SET);
	return len;
}

int tarchiver::parseoct(const char *p, size_t n) {
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
	return i;
}

int tarchiver::is_end_of_archive(const char *p) {
	int n = 0;
	for (n = 511; n >= 0; --n){
		if (p[n] != '\0'){
			return 0;
		}
	}
	return 1;
}


FILE *tarchiver::create_file(char *name) {
	FILE *f=NULL;
	f = fopen(name, "wb+");
	if (f == NULL) {
		char *p = strrchr(name, '/');
		if (p != NULL) {
			f = fopen(name, "wb+");
		}
	}
	return f;
}

int tarchiver::verify_checksum(char *p) {
	int n=0, u = 0;
	for (n = 0; n < 512; ++n){
		if (n < 148 || n > 155){
			u += (reinterpret_cast<unsigned char*>(p))[n];
		}
		else {
			u += 0x20;
		}
	}
	return (u == parseoct(p + 148, 8));
}


void tarchiver::untar(FILE *a, const char *name) {
	char buff[512];
	FILE *f = NULL;
	size_t bytes_read;
	int filesize=0;
	string my(name);

	logging(("Extracting from " + my).c_str());

	while (true) {
		bytes_read = fread(buff, 1, 512, a);

		if (bytes_read < 512){
			return;
		}

		if (is_end_of_archive(buff)) {
			logging(("End of " + my).c_str());
			return;
		}

		if (!verify_checksum(buff)) {
			logging("Checksum failure");
			return;
		}

		filesize = parseoct(buff + 124, 12);
		string my(buff);
		logging(("Extracting file " + my).c_str());
		f = create_file(buff);

		while (filesize > 0){
			bytes_read = fread(buff, 1, 512, a);
			if (bytes_read < 512){
				return;
			}

			if (filesize < 512){
				bytes_read = filesize;
			}

			if (f != NULL) {
				if (fwrite(buff, 1, bytes_read, f) != bytes_read) {
					logging("Failed write ");
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

int tarchiver::extract(const char *tarFileName) {
	FILE *a;
	a = fopen(tarFileName, "rb");
	if (a == NULL) {
		string my(tarFileName);
		logging(("Unable to open "+my).c_str());
	}

	else {
		untar(a, tarFileName);
		fclose(a);
	}

	return 0;
}

void tarchiver::logging(const char *msg) {
	if (log == 0) {
		ofstream fout_log;
		fout_log.open("log.txt", std::ios_base::app);
		fout_log << msg << endl;
		fout_log.close();
	}

	if (log == 1) {
		cout << msg << endl;
	}
}
