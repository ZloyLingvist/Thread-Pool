### Описание ###
Библиотека тредпул+архиватор

### Тестирование ###
Пусто

### getting-started ###

``` cpp
task t1;
bool v = false;//показывать ли подробные сообщения
TaskQueue queue(q, v); //q- длина очереди задач
t1.add_function(compress_own, "newarch.tar","in1.txt");	
queue.add_task("Add and Compress (own)",5,t1); //название, приоритет и экземпляр структуры task
Thread_pool thread_pool(w, queue, v); //w -- число тредов
```
### Дополнительные зависимости ###

В программе используется библиотека quicklz http://www.quicklz.com/

### Недостатки ###

В обязательном порядке у вызываемой функции должен быть фиктивный параметр int
# Пример #
```cpp
void myfunc(int i,const char *in){
	cout << in << endl;
}
...
task t1;
t1.add_function(myfunc, "text");	

```
