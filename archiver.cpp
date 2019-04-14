#pragma once
#include "archive.h"

Tar::Tar(const char *filename) : _finished(false) {
		this->out = std::fopen(filename, "wb");
		if (out == NULL){
			throw("Cannot open output");
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
	while (p < static_cast<tarheader*>(header)->checksum){
		sum += *p++ & 0xff;
	}
	
	for (int i = 0; i < 8; ++i){
		sum += ' ';
		++p;
	}

	while (p < q){
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


void Tar::add_to_empty(const char* filename, const char* content) {
	tarheader header;
	_init((void*)&header);
	_filename((void*)&header, filename);
	header.filetype[0] = 48;
	_size((void*)&header, std::strlen(content));
	_checksum((void*)&header);
	std::fwrite((const char*)&header, sizeof(char), sizeof(tarheader), out);
	std::fwrite(content, sizeof(char), std::strlen(content), out);
	_endRecord(std::strlen(content));
}


void Tar::add_to_archive(const char* filename, const char* nameInArchive) {
	std::FILE* in = std::fopen(filename, "rb");
	if (in == NULL) {
		throw(10);
	}

	std::fseek(in, 0L, SEEK_END);
	long int len = std::ftell(in);
	std::fseek(in, 0L, SEEK_SET);

	tarheader header;
	_init((void*)&header);
	_filename((void*)&header, nameInArchive);
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

///проверяем есть ли еще заголовок для чтения
bool Tar::nextHeader(ifstream& inputFile, int nextHeaderBlock, int lengthOfFile, int position) {
	position += (nextHeaderBlock * 512);
	if (position < lengthOfFile)
		inputFile.clear();
	
	if (inputFile.eof())
		return false;
	
	//перейти к следующему заголовку
	inputFile.seekg((nextHeaderBlock * 512));

	//проверяем содержимое следующего заголовка
	if (inputFile.peek() == '\0' && inputFile.good())
		return false;

	if (inputFile.good() && !inputFile.eof())
		return true;

	return false;
}

tarheader Tar::readHeader(ifstream& inputFile, int nextHeaderBlock, int position) {
	tarheader header;
	inputFile.read((char *)&header, sizeof(header));
	position += 512;
	return header;
}

int Tar::convertSizeToInt(char size[12]) {
	int sizeInt = strtol(size, NULL, 8);
	return sizeInt;
}

void Tar::writeBody(ifstream& inputFile, tarheader& header) {
	ofstream outFile;
	int size = convertSizeToInt(header.filesize);
	string name = header.filename;
	char* body = new char[size];
	vector<char> bodyV;
	bodyV.resize(size);
	inputFile.read(&bodyV[0], size);
	bodyV[size - 1] = '\0';
	outFile.open(name);

	for (size_t i = 0; i < bodyV.size(); i++) {
		outFile << bodyV[i];
	}

	outFile.close();
	std::cout << "Created File: " << name.c_str() << endl;
}

int Tar::extract(string& tarFileName) {
	ifstream inputFile;
	int nextHeaderBlock = 0;
	inputFile.open(tarFileName);

	inputFile.seekg(0, inputFile.end);
	int lengthOfFile = inputFile.tellg();
	inputFile.seekg(0, inputFile.beg);
	
	int position = 0;

	if (inputFile.good()) {
		while (nextHeader(inputFile, nextHeaderBlock, lengthOfFile, position)){
			tarheader header = readHeader(inputFile, nextHeaderBlock, position);
			nextHeaderBlock += ((511 + convertSizeToInt(header.filesize)) / 512) + 1; 
			int checksum = strtol(header.checksum, NULL, 8);
			for (int i = 0; i < 8; i++) {
				header.checksum[i] = ' ';
			}

			int computedChecksum = 0;
			for (int i = 0; i < sizeof(header); i++) {
				computedChecksum += ((char*)&header)[i];
			}
			
			if (computedChecksum == checksum)
				writeBody(inputFile, header);
		} ;

		inputFile.close();
		cout << "Done with extraction of " << tarFileName.c_str() << endl;
	}
	else {
		cout << "Error extracting file. Could not open source file" << endl;
	}

	return 0;
}

bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath){
	std::ifstream lFile(lFilePath.c_str(), std::ifstream::in | std::ifstream::binary);
	std::ifstream rFile(rFilePath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!lFile.is_open() || !rFile.is_open())
	{
		return false;
	}

	char *lBuffer = new char[1024]();
	char *rBuffer = new char[1024]();

	do {
		lFile.read(lBuffer, 1024);
		rFile.read(rBuffer, 1024);

		if (std::memcmp(lBuffer, rBuffer,1024) != 0)
		{
			delete[] lBuffer;
			delete[] rBuffer;
			return false;
		}
	} while (lFile.good() || rFile.good());

	delete[] lBuffer;
	delete[] rBuffer;
	return true;
}

