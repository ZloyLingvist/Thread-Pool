#pragma once

#include "tar.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include <cstdio>

/**
* @file archiver.h
* @brief Класс архиватор
*/

namespace my
{

struct tarheader {
	char filename[100];
	char filemode[8];
	char userid[8];
	char groupid[8];
	char filesize[12];
	char modiftime[12];
	char checksum[8];
	char filetype[1];
	char linkname[100];

	char indicator[6];
	char version[2];
	char username[32];
	char groupname[32];
	char majornumber[8];
	char minornumber[8];
	char prefix[155];
	char pad[12];
};


class tarchiver{
protected:
	std::FILE* out; // Си ??
	static constexpr size_t TAR_BUFFER_SIZE = 5120;
	bool finished;
	bool closeFile;
	long int fileLength(FILE *file);
	tarheader header;
	int log = 0;

	void init(void* header); /// заполнение индикатора ustar, времени изменения, file mode и group id 
	void checksum(void* header);/// проверка контрольной суммы заголовка
	void size(void* header, unsigned long fileSize); /// заполнение поля размер файла 
	void filename_(void* header, const char* filename); /// заполнение имени файла (не более 100 символов)
	void endRecord(size_t len);/// проверяем, чтобы заголовок был кратен 512
	
	int verify_checksum(char *p);/// проверка контрольной суммы для распаковки
	std::FILE *create_file(char *name);
	int is_end_of_archive(const char *p);
	int parseoct(const char *p, size_t n);
	void untar(std::FILE *a, const char *name); /// 
	void logging(const char *msg); /// вывод на экран либо в файл 
	
private:
	std::ifstream inputStream;

public:
	/*!
	\param[in] filename Имя архива
	\param[in] mode Режим работы. "a" - используется для создания архива. "e" - для разархивирования
	\param[out] md Режим логирования: 0 - вывод в log.txt ; 1 - вывод на экран
	*/

	tarchiver(const char *filename, const char *mode,int md);
	~tarchiver();

	/*!
	\param[in] filename Имя архива
	*/
	void add_to_archive(const char* filename);
	
	/*!
	\param[in] filename Имя архива, который нужно распаковать
	*/
	int extract(const char *tarFileName);
	void close();
};

}
