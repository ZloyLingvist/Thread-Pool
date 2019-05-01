#pragma once
#include <memory>
#include <string>
#include "..\\archiver\tar.h"
#include "..\\libarch\quicklz.h"

/**
* @file libarch.h
*/

#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif

namespace my{
inline bool FileExists(const std::string &Filename)
{
    return access(Filename.c_str(), 0) == 0;
}

class lib_archiver : public Tar
 {
public:
    int compress(const char* in, const char* out)
    {
        if (!FileExists(in))
            return -1;
        quick_compress(in, out);
        return 0;
    }
    int decompress(const char* in, const char* out) 
    {
        if (!FileExists(in))
            return -1;
        quick_decompress(in, out);
        return 0;
    }

};
}