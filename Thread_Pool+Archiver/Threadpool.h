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
    std::mutex lock{};
    std::condition_variable data_condition{};
    std::atomic<bool> active{};
    std::vector<std::thread> vector_thread_pool{};
    std::vector<std::function<void()>> myv{};
    int workers{ 0 }; // количество потоков
    bool v{ false }; // verbose показывать ли 
public:
    /*!
    По количеству потоков в vector_thread_pool закидываются тред. К которым на вход подаются задачи из TaskQueue
    Когда поток освободился он "просит" выдать себе задачу. Когда задачи выполнены, то работа завершается
    \param w -- количество потоков, TaskQueue -- очередь задач
    */
    Threadpool(int w, TaskQueue &obj, bool verbose);

    /*!
    По количеству потоков в vector_thread_pool закидываются тред. К которым на вход подаются задачи из вектора функций,
    Когда поток освободился он "просит" выдать себе задачу. Когда задачи выполнены, то работа завершается
    \param w -- количество потоков
    */
    Threadpool(int w, bool verbose);

    /*!
    Пока есть задачи треды их выполняют в цикле. Задачи берутся из TaskQueue
    */
    void work(TaskQueue &obj);

    /*!
    Проверка есть ли еще задачи в векторе задач TaskQueue. Если есть, то добавляем задачи в очередь
    */
    bool run(TaskQueue &obj);

    /*!
    Пока есть задачи треды их выполняют в цикле. Задачи берутся из вектора задач тредпула
    */
    void simple_run();

    /*!
    Добавление функций в вектор задач тредпула
    */
    template <typename F, typename... Rest>
    auto add_function(F&& f, Rest&&... rest) -> std::future<typename std::result_of<F(Rest...)>::type>
    {
        using namespace std;
        using Return = typename result_of<F(Rest...)>::type;
        using Functor = Return();
        auto pck = make_shared<packaged_task<Functor>>(bind(forward<F>(f), forward<Rest>(rest)...));
    
        auto func = [pck]() { (*pck)(); };
        myv.push_back(func);
        return pck->get_future();
    }

    Threadpool::~Threadpool();
};

}
