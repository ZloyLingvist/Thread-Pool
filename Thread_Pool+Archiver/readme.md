### Описание ###
Простейшая реализация библиотеки Thread_Pool, взаимодействующая с TaskQueue (очередь задач). Для тестирования реализован простейший архиватор, который умеет упаковывать файлы в tar-формат, использовать внутренние алгоритмы сжатия и расжатия LZW и алгоритмы библиотеки quicklz <br />

Реализованы следующие классы:
##### Tar (абстрактный класс) #####
###### lib_archiver - библиотечный класс (сжатие и расжатие) ######
###### lzw_archiver - сжатие и расжатие (алгоритм LZW). Архивация и деархивация (tar-формат) ######
##### TaskQueue - класс для раздачи задач #####
##### ThreadPool - класс реализующий функции Thread Pool #####
##### Testing_class - тестирующий класс #####

### getting-started ###
Пример запуска может быть следующим: <br />
1. Раздать свою функцию на все треды <br />
``` cpp
Threadpool tp(w,max_error, v);//w - максимальное число потоков. max_error-максимально допустимое число ошибок, bool v отображать ли работу программы на экране <br />
auto future1=tp.append_function(examp, 1, 2); //подали функцию examp, принимающую два параметра типа int
```
2. Запустить тредпул и taskqueue вместе <br />
``` cpp
int workers=2; //- число потоков
int q=1; //- размер очереди
task t1; //-- создаем задачу
int max_errors = 2;// -- указываем сколько раз может произойти ошибка
bool v = false; //-- решим отображения
string tn1 = "Task1"; //-- имя задачи
auto future1 = t1.add_task(tn1, 1, my_write,"T1\n", 1); //-- задача выполнить функцию my_write с именем tn1, приоритетом 1 и параметрами const char * и int <br />
TaskQueue queue(q, v); 
queue.push(t1); //положили задачу в очередь
Threadpool thread_pool(w, queue,max_errors,v);//запустили тредпул
```

3. Архиватор <br />
##### 3.1 Создание архива #####
``` cpp
tarchiver myar(name,"a",log);//будем добавлять файлы в архив name. log (0 || 1) - режим логирования "a|e" - добавить/извлечь
ownarchiver o_ar; ( lib_archiver l_ar) 
o_ar.compress(file, file_tmp);//сжимаем файл с именем file и называем его file_tmp
myar.add_to_archive(file_tmp);//добавили сжатый файл в архив
myar.close();//закрыли архив
```
##### 3.2 Извлечение файлов #####
``` cpp
tarchiver myar2(name,"e",log);//вытаскиваем файлы(сжатые). log (0 || 1) - режим логирования "a|e" - добавить/извлечь
ownarchiver o_ar; ( lib_archiver l_ar) 
o_ar.decompress(file_tmp, file_res);//рассжимаем файл с именем file_tmp и называем его file_res
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
1. При попытке запустить свою функцию на тредпул: может зависнуть. Скомпилировать проект еще раз. Так же возможно, то что отработает меньшее число тредов чем должно было бы
2. При Release x64 замечен баг, что для получения корректной работы нужно запускать глобальный тест отдельно от остальных
3. Сильно усложненные для восприятия unit-тесты

### Необходимые знания ###
https://en.wikipedia.org/wiki/Tar_(computing) <br />
https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%9B%D0%B5%D0%BC%D0%BF%D0%B5%D0%BB%D1%8F_%E2%80%94_%D0%97%D0%B8%D0%B2%D0%B0_%E2%80%94_%D0%92%D0%B5%D0%BB%D1%87%D0%B0

### Лицензия ###
Использование в некоммерческих целях. 

### Планы ###
Планируется развивать проект и дальше. За найденные замечания буду благодарен

### Автор ###
Igor Butuzov. April 2019
