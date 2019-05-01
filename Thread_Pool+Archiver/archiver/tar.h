#pragma once

/**
* @file tar.h
* @brief Абстрактный класс архиватор
*/
namespace my
{
class Tar
{
public:
    virtual ~Tar() {}
    virtual int compress(const char *input, const char *output) = 0;
    virtual int decompress(const char *input, const char *output) = 0;
};
}
