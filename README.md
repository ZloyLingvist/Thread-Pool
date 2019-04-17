# Simple Thread_Pool
Простейшая реализация пула потоков на C++

Для подключения библиотеки в VS зайти в: Properties/Linker/Input/Additional Dependencies добавить getopt.lib (для 32 бит файл прилагается)

Для x64 скомпилировать getopt.c и getopt.h в lib-файл

# Thread_pool+Archiver
UPD 15.04.2019 
Добавлен упаковщик("сжать" архив алгоритмом LZW, положить в архив, извлечь из архива, расжать, проверить на соответствие начальному).

В программе используется библиотека zlib. 
Подключение (VS 2015) 
Properties-Linker-General-Additional Library Directories: path/zlib
Properties-Linker-Input-Additional Dependencies: zlib.lib

Баги на данный момент не отлажены


