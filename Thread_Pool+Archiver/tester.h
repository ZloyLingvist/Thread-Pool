#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#include "tar.h"
#include "archiver.h"
#include "libarch.h"
#include "ownarchiver.h"
#include "TaskQueue.h"

/**
* @file tester.h
* @brief Тестирующий класс
*/

namespace my
{

namespace test
{

class Testing_class
{
private:
    std::vector<std::string> test_vector; //здесь хранятся имена файлов для которых нужно провести тестирование. Первый элемент-имя архива
                                //если последнее значение вектора 1, то без сжатия; 2 (не библиотечный алгоритм сжатия). 3 библиотечный
    double size_after = 0, size_before = 0; // размер файлов до и после

    std::string creativer(); //Генерирует случайную строку. Используется для заполнения файла с произвольным именем 
                        // произвольным содержимым
                        //return сгенерированную строку (тип string )

    bool exists(const char *name); //проверка файла на существование
    double filesize(const char* filename); //размер файла
    void creation_function(int p, int log); //функция добавления в архив. p=1 || 4 без сжатия, p==2 не библиотечное сжатие , p==3 библиот.
    bool extraction_function(int p, int log);//функция извлечения из архива. p==1 извлечь. p==2 извлечь и декомпресс (не библ) p==3 извлечь и декомпресс (библ)
    void create_file(int mode, std::vector<std::string> &filenames);//создает файлы в соответствии с filenames. 
                                                          //Если mode=0, то используются имена введенные пользователем, 
                                                          //иначе сгенерированный программой

    void cleaner();//после завершения работы удаляет файлы в соответствии с именами в векторе test_vector

    bool sub_test_3();//Функция test_3 получает произвольное число имен с которыми хочет работать пользователь. 
                      //sub_test_3 архивирует и дезархивирует эти файлы. Успех или не успех возвращаются в test_3

                      // используется в test_3 для разделения введенного пользователем на отдельные сегменты
    std::string merge(std::initializer_list<std::string> strList)
    {
        std::string ret = "";
        for (std::string s : strList) { ret += s; }
        return ret;
    }

protected:
    int log_test = 0;//вывести на экран (0) или в файл(1) процесс тестирования

public:
    Testing_class(int lg)
    {
        log_test = lg;
    }

    ~Testing_class() = default;

    /*!
    Логирование. В зависимости от значения log_test куда вывести процесс тестирования ( сообщение msg )
    */

    void logging(const char *msg);

    /*!
    Сравнивает 2 файла с именами lFilePath и rFilePath.
    \return true если файлы эквивалентны. false иначе
    */
    bool isFilesEqual(const std::string& lFilePath, const std::string& rFilePath);

    /*!
    Создать архив и проверить его на существование
    \param p - 1 сгенерировать файл с произвольным именем и произвольным содержимым
    \return true если успех. false иначе
    */

    bool test_1(int p);

    /*!
    Создать архив, извлечь его содержимое и проверить, что вес изначальных файлов равен весу извлеченных
    \param p p==1 извлечь. p==2 извлечь и декомпресс (не библ) p==3 извлечь и декомпресс (библ)
    \return true если успех. false иначе
    */
    bool test_2(int p);

    /*!
    Создать архив, извлечь его содержимое и проверить, что вес изначальных файлов равен весу извлеченных
    \param p p=1 сгенерировать файл с произвольным именем и произвольным содержимым
    \return true если успех. false иначе
    */
    template<typename T>
    std::string toString(T value)
    {
        ostringstream oss;
        oss << value;
        test_vector.push_back(oss.str());
        return oss.str() + " ";
    }

    template< typename ... Args >
    bool test_3(const Args& ... args)
    {
        merge({ toString(args)... });
        if (sub_test_3() == true)
        {
            return true;
        }
        else
        {
            return false;
        }
    }


    bool test_5(const char* in, const char* out);// файл in сжать и разжать (выходной out) с помощью библиотеки. 
                                                 // Сравнить in и out на эквивалентность
    bool test_7(int q);//проверка TaskQueue. На вход подается длина очереди и задачи архиватора.
    bool test_8(int w);//проверка ThreadPool. На вход подается число потоков
    bool test_9(int w, int q);//проверка TaskQueue+ThreadPool на простой функции
    bool test_10(int w, int q);//проверка TaskQueue+ThreadPool+Archiver
};

/*!
\param i,in,count- i-фиктивный параметр. Напечатать in count раз. Используется в тесте 9
*/
void my_write(/*int i, */const char *in, int count);

/*!
Используется для тестирования тредпула на произвольных функциях
*/
void void_task(/*int i, */int a, int b, int n, const char *text);
/*!
Используется для тестирования тредпула на произвольных функциях
*/
int int_task(/*int i, */int a, int b);
/*!
Используется для тестирования тредпула на произвольных функциях
*/
std::string string_test(/*int i, */const char *str1, const char *str2);

/*!
Загрузить queue задачами; log-вывод на экран или файл
*/
void function_block(TaskQueue &queue, int log);

/*!
Удаление файлов name, in1, in2, in3
*/
void removing(const char *name, const char *in1, const char *in2, const char *in3);

/*
Извлечь файл name
*/
void extract(/*int i, */int log, const char *name);

/*
Не библиотечное сжатие файлов ... и добавление их в архив name
*/
void compress_own(/*int i, */int log, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2);

/*
Библиотечное сжатие файлов ... и добавление их в архив name
*/
void compress_lib(/*int i, */int log, const char *name, const char *file1, const char *file_tmp1, const char *file_tmp2, const char *file2);

/*
Не библиотечное расжатие файлов
*/

void decompress_own(/*int i, */const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2);

/*
Не библиотечное расжатие файлов
*/
void decompress_lib(/*int i, */const char *name, const char *file_in1, const char *file_in2, const char *file_out1, const char *file_out2);

/*
Сравнить файлы file1 и file_res1; file2 и file_res2
*/
void comp(/*int i, */const char *file1, const char *file2, const char *file_res1, const char *file_res2);

}
}