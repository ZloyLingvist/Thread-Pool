#include "tar.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath);

class RLECompression {
	int runLength;
	int nextByte;
	int previousByte;
public:
	RLECompression(int mode, ifstream& inFile, ofstream& outFile){
		if (mode == 0)
			compress(inFile, outFile);
		else if (mode == 1)
			decompress(inFile, outFile);
	}

	void compress(ifstream& inFile, ofstream& outFile) {
		nextByte = inFile.get();
		previousByte = nextByte;

		while (nextByte != EOF) {
			runLength = 1;
			previousByte = nextByte;

			while ((nextByte = inFile.get()) != EOF && nextByte == previousByte)
				runLength++;

			outFile.put(runLength);
			outFile.put(previousByte);
		}
	}
	void decompress(ifstream& inFile, ofstream& outFile) {
		while ((runLength = inFile.get()) != EOF) {
			nextByte = inFile.get();

			for (int i = 0; i < runLength; i++)
				outFile.put(nextByte);
		}
	}
};


