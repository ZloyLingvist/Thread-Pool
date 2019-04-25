#pragma once
#include <memory>
#include "tar.h"
#include "quicklz.h"

class lib_archiver:public Tar {
public:
	lib_archiver() = default;
	~lib_archiver() = default;
	int compress(const char* in, const char* out) override;
	int decompress(const char* in, const char* out) override;
};

