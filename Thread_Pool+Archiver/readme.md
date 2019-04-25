[Описание][]
[Тестирование][]
[getting-started][]
[Дополнительные зависимости][]

### Описание ###
Библиотека тредпул+архиватор
### Тестирование ###
Пусто

### getting-started ###
```
task t1;
		bool v = false;//показывать ли подробные сообщения
		TaskQueue queue(q, v); //q- длина очереди задач
		t1.add_function(compress_own, "newarch.tar","in1.txt");	
		queue.add_task("Add and Compress (own)",5,t1); //название, приоритет и экземпляр структуры task
    Thread_pool thread_pool(w, queue, v); //w -- число тредов
```
### Дополнительные зависимости ### 
В программе используется библиотека quicklz http://www.quicklz.com/
