### Описание ###
Простейшая реализация Thread_Pool, взаимодействующая с TaskQueue (очередь задач). Для тестирования реализован простейший архиватор, который умеет упаковывать файлы в tar-формат, использовать внутренние алгоритмы сжатия и расжатия LZW и алгоритмы библиотеки quicklz <br />

Реализованы следующие классы:
##### Tar (абстрактный класс) #####
###### lib_archiver - библиотечный класс (сжатие и расжатие) ######
###### lzw_archiver - сжатие и расжатие (алгоритм LZW). Архивация и деархивация (tar-формат) ######
##### TaskQueue - класс для раздачи задач #####
##### ThreadPool - класс реализующий функции Thread Pool #####
##### Testing_class - тестирующий класс #####

### getting-started ###
Пример запуска может быть следующим:
``` cpp
task t1;
bool v = false;//показывать ли подробные сообщения
TaskQueue queue(q, v); //q- длина очереди задач
t1.add_function(compress_own,log,"newarch.tar","in1.txt");	
queue.add_task("Add and Compress (own)",5,t1); //название, приоритет и экземпляр структуры task
Thread_pool thread_pool(w, queue, v); //w -- число тредов
```

### Тестирование ###
Тест 1 - тестирование отдельных функций архиватора
``` cpp
void testing_block_1(int log) {
	try {
		Testing_class obj(log);
		cout << "=====Result of T1 is " << obj.test_1(1) << endl;
		cout << "=====Result of T2 is " << obj.test_2(2) << endl;
		cout << "=====Result of T3 is " << obj.test_3("my.tar", "in1.txt", "in2.txt", "2") << endl;//2 или 3 выбор алгоритма сжатия
		cout << "=====Result of T3_2 is " << obj.test_3("my.tar", "in1.txt", "in2.txt", "3") << endl;
		cout << "=====Result of T4 is " << obj.test_2(3) << endl;
		cout << "=====Result of T5 is " << obj.test_5("in1.txt", "in2.txt") << endl;
	}
	catch (const std::exception &e) {
		cout << "При тестировании произошла ошибка" << endl;
	}
}

```
Тест 2 - тест тестирующий TaskQueue , ThreadPool and Archiver
``` cpp
void testing_block_2(int w,int q,int log) {
	try {
		Testing_class obj(log);
		cout << " =====Result of T10 is " << obj.test_10(w, q) << endl;
	}
	catch (const std::exception &e) {
		cout << "При тестировании произошла ошибка" << endl;
	}
}

```

### Дополнительные зависимости ###

MS Visual Studio 2015 <br />
Библиотека quicklz http://www.quicklz.com/

### Недостатки ###
1. Если линкер не позволит собрать проект, то возможное решение в файле libarch.cpp закомментировать вызов quick_decompress(in, out); and quick_compress(in, out); Собрать проект. Перед следующим вызовом восстановить эти функции
2. В обязательном порядке у вызываемой функции должен быть фиктивный параметр типа int
#### Пример ####
```cpp
void myfunc(int i,const char *in){
	cout << in << endl;
}
...
task t1;
t1.add_function(myfunc, "text");	

```
### Необходимые знания ###
https://en.wikipedia.org/wiki/Tar_(computing)
https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%9B%D0%B5%D0%BC%D0%BF%D0%B5%D0%BB%D1%8F_%E2%80%94_%D0%97%D0%B8%D0%B2%D0%B0_%E2%80%94_%D0%92%D0%B5%D0%BB%D1%87%D0%B0

### Лицензия ###
Использование в некоммерческих целях. 

### Планы ###
Планируется развивать проект и дальше. За найденные замечания буду благодарен

### Автор ###
Igor Butuzov April 2019
