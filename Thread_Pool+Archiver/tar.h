#pragma once

/**
* @file tar.h
* @brief ����������� �����-���������
*/
namespace my
{
class Tar
{
public:
    //Tar()=default;
    virtual ~Tar() {}
    virtual int compress(const char *input, const char *output) = 0;
    virtual int decompress(const char *input, const char *output) = 0;
};
}
