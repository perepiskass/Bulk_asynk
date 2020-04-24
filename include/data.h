#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <csignal>
#include <algorithm>
#include <queue>
#include <typeinfo>
#include <cstring>
#include <sstream>
#include <atomic>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "pcout.h"


// using Time = std::chrono::seconds;
// using Bulk = std::pair<std::vector<std::string>,Time>;


class Observer {
public:
    virtual void setBulk(const Bulk&) = 0;
    virtual void update(int id) = 0;
    virtual ~Observer() = default;
};

using Subscrabers = std::vector<Observer*>;


class DataIn
{
public:
    DataIn(int size);
    ~DataIn();
    void setBulk(std::size_t bulk);
    void subscribe(Observer *obs);
    void setData(std::string&& str);
    void notify();

    std::vector<std::thread*> vec_thread;
    std::queue<Bulk> bulkQ;
    std::condition_variable cv;
    std::mutex mtx_cmd;
    std::mutex mtx_file;
    std::atomic<bool> works;

private:
    void clearData();
    void checkDilimiter(std::string& str);
    void setQueues();

    Subscrabers subs;
    std::pair<bool,uint8_t> checkD; ///< переменная для проверки использования знаков динамического разделения блоков "{" и "}" и хранения состояния о их кол-ве
    Bulk bulk;
    std::size_t count;        ///< хранит информацию о размере блока, задаеться при запуске программы (инициализируеться в конструкторе)
    std::size_t countTry;           ///< оставшееся ко-во команд для ввода в блок для его формирования
};

class DataToConsole:public Observer
{
    private:
    std::shared_ptr<DataIn> _data;
    std::queue<Bulk> bulkQ;
    public:
        void setBulk(const Bulk& bulk) override;
        DataToConsole(std::shared_ptr<DataIn> data);
        ~DataToConsole()override;
        void update(int id);
};

class DataToFile:public Observer
{
    private:
    std::shared_ptr<DataIn> _data;
    std::queue<Bulk> bulkQ;

    public:
        void setBulk(const Bulk& bulk) override;
        DataToFile(std::shared_ptr<DataIn> data);
        ~DataToFile()override;
        void update(int id);

};

