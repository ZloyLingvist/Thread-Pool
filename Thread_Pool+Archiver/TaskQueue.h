#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <queue>

/**
* @file TaskQueue.h
* @brief Описание очереди задач и структуры задача
*/

namespace my
{

struct task
{
private:
    int id_ = 0;
    int priority = 0;
    std::string name{};
    bool doing{ false };
    std::function<void()> func{};
public:
    task() = default;
    ~task() = default;
    /*!
    Переопределяем оператор для использования в priority_queue
    */
    bool operator<(const task &other) const { return priority > other.priority; }

    /*!
    Функция нужная для добавления произвольной функции с произвольным числом параметров в task.
    Недостаток в том, что первый аргумент функции должен быть фиктивным и типа int
    */
    template <typename F, typename... Rest>
    auto add_function(F&& f, Rest&&... rest) -> std::future<typename std::result_of<F(Rest...)>::type>
    {
        using namespace std;
        using Return = typename result_of<F(Rest...)>::type;
        using Functor = Return();
        auto pck = make_shared<packaged_task<Functor>>(bind(forward<F>(f), forward<Rest>(rest)...));

        this->func = [pck]() { (*pck)(); };
        return pck->get_future();
    }

    /*!
    Возвращает id текущей задачи (task)
    */
    int task_id() const
    {
        return this->id_;
    }

    /*!
    Добавление названия и приоритета к текущей задаче (task)
    */

    void make_task(const char *name, int priority)
    {
        this->name = name;
        this->priority = priority;
    }

    /*!
    \return приоритет текущей задачи (task)
    */
    int task_priority() const
    {
        return this->priority;
    }

    /*!
    \return имя текущей задачи (task)
    */
    std::string task_name() const
    {
        return this->name;
    }

    /*!
    \return вернуть функцию текущей задаче (task)
    */

    std::function<void()> task_func() const
    {
        return func;
    }
};


class TaskQueue
{
private:
    std::size_t sz{};
    int quescap{};
    std::mutex m_mtx{};
    std::condition_variable m_cond{};
    mutable task t{};
    bool v = false;
    std::priority_queue<task> function_queue{};
    std::vector<task> task_vector{};
    task give_task() const;
public:
    TaskQueue(int q, bool verbose);

    TaskQueue(const TaskQueue& rhs) = delete;
    TaskQueue& operator=(const TaskQueue& rhs) = delete;

    TaskQueue(TaskQueue&&) = default;
    TaskQueue& operator=(TaskQueue&&) = default;

    /*!
    Добавить задачу с именем name и приоритетом p в TaskQueue
    */
    void add_task(const char *name, int p, task obj);

    /*!
    Добавить задачу в конец TaskQueue
    */
    void push_to_end(std::thread::id this_id);

    /*!
    Вывести информацию о текущем потоке 1.Задачу запросил 2.Задачу получил 3. Задачу не получил
    */

    void print(int mode, std::thread::id this_id) const;

    /*!
    Проверить очередь на пустоту
    */
    bool empty() const;

    /*!
    Удалить задачу из TaskQueue
    */
    void pop();

    /*!
    Проверить вектор задач на пустоту
    */
    bool check_task_vector() const;

    /*!
    \return текущий размер очереди задач
    */
    std::size_t size_function_queue() const;

    /*!
    \return максимальный размер очереди задач
    */
    int return_quescap();

    /*!
    Добавить задачу в очередь
    */
    void push(int k);

    /*!
    \return номер текущей задачи
    */
    int task_id() const;

    /*!
    \return функцию текущей задачи
    */
    std::function<void()> task_f() const;

    /*!
    \return имя текущей задачи
    */
    std::string task_name() const;

    /*!
    Брать задачи из вектора задач. Добавлять их в очередь и выполнять.
    */
    void simple_run();

    TaskQueue() = default;
};

}

