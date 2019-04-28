#pragma once
#include "tar.h"
#include <memory>

/**
* @file ownarchiver.h
* @brief Описание тестового класса
*/

struct dictionary{
	int code_value;
	int prefix_code;
	char character;
};

struct bi_file{
	/*! Побитовый доступ к файлам
	*/
	FILE *file;
	unsigned char mask;
	int rack;
	int pacifier_counter;
};

class ownarchiver :public Tar {
protected:
	static constexpr size_t BITS = 12; //количество битов в коде 
	static constexpr size_t MAX_CODE = ((1 << BITS) - 1);//максимальное значение кода
	static constexpr size_t TABLE_SIZE = 5021; //размер словаря элементов
	static constexpr size_t end_of_stream = 256; //код конца потока
	static constexpr size_t FIRST_CODE = 257; //значение кода получаемое первой добавленной в словарь фразой
	static constexpr size_t UNUSED = -1;//признак свободной ячейки в словаре
	static constexpr size_t PACIFIER_COUNT = 2047;
	static constexpr size_t CHUNK = 4096;
	static constexpr size_t dict_size = 5024;
public:
	char decode_stack[TABLE_SIZE];
	ownarchiver() = default;
	~ownarchiver() = default;
	int compress(const char* in, const char* out) override;
	int decompress(const char* in, const char* out) override;

	std::shared_ptr<bi_file> Open_File(const char *name, const char *mode);
	void Close_File(std::shared_ptr<bi_file> b, int mode);

	void WriteBits(std::shared_ptr<bi_file> bfile, unsigned long code, int count);
	unsigned long ReadBits(std::shared_ptr<bi_file> bfile, int bit_count);

	/*! процедура поиска в словаре указанной пары (код фразы, символ).
	используеться hash , получаемый из параметров
	*/
	unsigned int find_dictionary_match(int prefix_code, int character, dictionary *dict);
	unsigned int decode_string(unsigned int count, unsigned int code, dictionary *dict);
};