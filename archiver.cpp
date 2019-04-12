#include "tar.h"
#include <iostream>
#define TARHEADER static_cast<tarheader*>(header)

Tar::Tar(const char *filename,int mode) : _finished(false) {
	if (mode == 1) {
		this->out = std::fopen(filename, "wb");
		if (out == NULL) {
			throw("Cannot open output");
		}
		this->_closeFile = true;
		this->_finished = true;
		if (sizeof(tarheader) != 512) {
			throw(sizeof(tarheader));
		}
	}

	if (mode == 2) {
		this->usBlockSize = 512;
		this->fileNum = -1;
		this->inputStream.open(filename, ifstream::in | ifstream::binary);
		this->inputStream.seekg(0, ios::beg);
		this->_closeFile = true;
		this->_finished = true;
	}
}

Tar::~Tar() {
	if (!_finished) {
		throw("Tar file was not finished.");
	}
}

void Tar::_init(void* header) {
	std::memset(header, 0, sizeof(tarheader));
	std::sprintf(TARHEADER->magic, "ustar  ");
	std::sprintf(TARHEADER->mtime, "%011lo", time(NULL));
	std::sprintf(TARHEADER->mode, "%07o", 0777);
	std::sprintf(TARHEADER->gid, "%s", "users");
}

void Tar::_checksum(void* header) {
	unsigned int sum = 0;
	char *p = (char *)header;
	char *q = p + sizeof(tarheader);
	while (p < TARHEADER->checksum) sum += *p++ & 0xff;
	for (int i = 0; i < 8; ++i) {
		sum += ' ';
		++p;
	}
	while (p < q) sum += *p++ & 0xff;

	std::sprintf(TARHEADER->checksum, "%06o", sum);
}

void Tar::_size(void* header, unsigned long fileSize) {
	std::sprintf(TARHEADER->size, "%011llo", (long long unsigned int)fileSize);
}

void Tar::_filename(void* header, const char* filename) {
	if (filename == NULL || filename[0] == 0 || std::strlen(filename) >= 100) {
		//throw("invalid archive name \"" << filename << "\"");
		throw(20);
	}
	sprintf(TARHEADER->name, "%s", filename); // max 100 chars !!!
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

void Tar::put(const char* filename, const std::string& s) {
	put(filename, s.c_str(), s.size());
}

void Tar::put(const char* filename, const char* content) {
	put(filename, content, std::strlen(content));
}

void Tar::put(const char* filename, const char* content, std::size_t len) {
	tarheader header;
	_init((void*)&header);
	_filename((void*)&header, filename);
	header.typeflag[0] = 48;
	_size((void*)&header, len);
	_checksum((void*)&header);
	std::fwrite((const char*)&header, sizeof(char), sizeof(tarheader), out);
	std::fwrite(content, sizeof(char), len, out);
	_endRecord(len);
}

void Tar::putFile(const char* filename, const char* nameInArchive) {
	std::FILE* in = std::fopen(filename, "rb");
	if (in == NULL) {
		//throw("Cannot open " << filename << " "<< std::strerror(errno));
		throw(10);
	}

	std::fseek(in, 0L, SEEK_END);
	long int len = std::ftell(in);
	std::fseek(in, 0L, SEEK_SET);

	tarheader header;
	_init((void*)&header);
	_filename((void*)&header, nameInArchive);
	header.typeflag[0] = 0;
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

void Tar::showContent(const char *file) {
	while (this->inputStream) {
		struct tarheader buffer;
		if (this->inputStream.read((char*)&buffer, this->usBlockSize)) {
			this->tarVector.push_back(buffer);
			int fileSize = this->hex2Dec(buffer.size, sizeof(buffer.size));
			int jumpBlock = ceil((double)fileSize / (double)this->usBlockSize);
			this->inputStream.seekg(jumpBlock * this->usBlockSize, ios::cur);
		}
	}

	cout << "Total " << this->getFileNum() << " " << (this->getFileNum() > 1 ? "files." : "file.") << endl;
	for (int i = 0; i < this->getFileNum(); i++) {
		struct tarheader buffer = this->tarVector.at(i);
		cout << left << buffer.name << endl;
	}
	this->inputStream.close();
}

int Tar::getFileNum() {
	if (this->fileNum >= 0) {
		return this->fileNum;
	}

	for (int i = this->tarVector.size() - 1; i > -1; i--) {
		bool zeroFlag = true;
		struct tarheader buffer = this->tarVector.at(i);

		for (int j = 0; j < (int)sizeof(buffer.checksum); j++) {
			if (buffer.checksum[j] != 0x00) {
				zeroFlag = false;
				break;
			}
		}

		if (!zeroFlag) {
			this->fileNum = i + 1;
			break;
		}
	}

	return this->fileNum;
}

int Tar::hex2Dec(const char* sizeArray, int length) {
	int n = 0;
	int ans = 0;
	for (int i = length - 2; i > -1; i--) {
		int num = sizeArray[i] >= 'a' && sizeArray[i] <= 'f' ? sizeArray[i] - 'a' + 10 : sizeArray[i] - '0';
		ans += num * pow(16, n);
		++n;
	}
	return ans /= 8;
}

bool Tar::nextHeader(ifstream& inputFile, int nextHeaderBlock, int lengthOfFile, int position) {
	position += (nextHeaderBlock * 512);
	if (position < lengthOfFile)
		inputFile.clear();
	
	if (inputFile.eof())
		return false;
	
	inputFile.seekg((nextHeaderBlock * 512));

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
	int size = convertSizeToInt(header.size);
	string name = header.name;
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

int Tar::extract() {
	ifstream inputFile;
	int nextHeaderBlock = 0;
	string tarFileName = "archive.tar";
	inputFile.open(tarFileName);
	inputFile.seekg(0, inputFile.end);
	int lengthOfFile = inputFile.tellg();
	inputFile.seekg(0, inputFile.beg);
	int position = 0;

	if (inputFile.good()) {
		do {
			//read header
			tarheader header = readHeader(inputFile, nextHeaderBlock, position);
			nextHeaderBlock += ((511 + convertSizeToInt(header.size)) / 512) + 1; 
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

		} while (nextHeader(inputFile, nextHeaderBlock, lengthOfFile, position));

		inputFile.close();
		cout << "Done with extraction of " << tarFileName.c_str() << endl;
	}
	else {
		cout << "Error extracting file. Could not open source file" << endl;
	}

	system("pause");
	return EXIT_SUCCESS;
}


int main() {

	/* создали архив */
	Tar tar("archive.tar", 2);
	//tar.put("11.txt", "Hello World 1\n");
	//tar.putFile("44.txt", "4.txt");
	//tar.putFile("51.txt", "5.txt");
	//tar.close();
	tar.showContent("archive.tar");
	//tar.extract();
	system("pause");
	return EXIT_SUCCESS;
}