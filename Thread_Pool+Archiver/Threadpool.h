#pragma once

#include <queue>
#include <future>
#include <mutex>
#include <functional>
#include <type_traits>
#include <iostream>
#include <ctime>  
#include <string>
#include "TaskQueue.h"

/**
* @file Threadpool.h
* @brief Класс реализующий функциональность тредпула
*/

namespace my
{

	class Threadpool
	{
	private:
		int max_errors = 0;/// максимально допустимое число ошибок
		unsigned int w = 0;// число потоков
		std::vector<std::unique_ptr<std::thread>> threads;
		std::vector<std::shared_ptr<std::atomic<bool>>> flags;
		std::atomic<bool> isDone;
		std::atomic<bool> isStop;
		std::atomic<int> waiting_thread;  // сколь тредов ждут
		std::mutex mutex;
		std::condition_variable data_condition;
		bool v{ false }; // verbose показывать ли
		int size();
		std::function<void()> func{};
		std::vector<std::thread> vector_thread_pool;

	public:
		Threadpool(unsigned int w, TaskQueue &queue, int errors, bool verbose);
		Threadpool(unsigned int workers, int maximim_errors, bool verbose);
		~Threadpool();

		/*!
		Завершение работы
		*/
		void stop(bool isWait);

		/*!
		Треды берут задачи из TaskQueue и выполняют их. Если очередь пуста то останавливаемся
		*/
		void work(TaskQueue &queue, int i);

		/*!
		Раздаем функции пользователя на все потоки
		*/

		template <typename F, typename... Rest>
		auto append_function(F&& f, Rest&&... rest)->std::future<typename std::result_of<F(Rest...)>::type>
		{
			using namespace std;
			using Return = typename result_of<F(Rest...)>::type;
			using Functor = Return();
			auto pck = make_shared<packaged_task<Functor>>(bind(forward<F>(f), forward<Rest>(rest)...));

			func = [pck]() { (*pck)(); };
		
			for (int i = 0; i < w; i++){
				vector_thread_pool.emplace_back(std::thread(&Threadpool::simple_run, this, std::ref(func)));
			}

			return pck->get_future();
		}

		/*!
		Функция которая выполняет функцию, которую дал пользователь
		\bug не всегда количество результатов функции совпадает с числом потоков. Может застопориться
		*/
		void simple_run(std::function<void()> a); 
};

}
