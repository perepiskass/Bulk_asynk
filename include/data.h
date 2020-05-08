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

class Observer {
public:
    virtual void setBulk(const Bulk&) = 0;
    virtual void update(size_t id) = 0;
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
    void write();
    void notify();

    std::vector<std::thread*> vec_thread;
    std::queue<Bulk> bulkQ;
    std::condition_variable cv;
    std::mutex mtx_input;
    std::mutex mtx_cmd;
    std::mutex mtx_file;
    std::atomic<bool> works;

private:
    void clearData();
    void checkDilimiter(std::string& str);
    void setQueues();

    Subscrabers subs;
    std::pair<bool,uint8_t> checkD; ///< переменная для проверки использования знаков динамического разделения блоков "{" и "}" и хранения состояния о их кол-ве
    Bulk* bulk;
    std::size_t count;        ///< хранит информацию о размере блока, задаеться при запуске программы (инициализируеться в конструкторе)
    std::size_t countTry;           ///< оставшееся ко-во команд для ввода в блок для его формирования
};

class DataToConsole:public Observer
{
    private:
    DataIn* _data;
    std::queue<Bulk> bulkQ;
    public:
        void setBulk(const Bulk& bulk) override;
        DataToConsole(DataIn* data);
        ~DataToConsole()override;
        void update(size_t id);
};

class DataToFile:public Observer
{
    private:
    DataIn* _data;
    std::queue<Bulk> bulkQ;

    public:
        void setBulk(const Bulk& bulk) override;
        DataToFile(DataIn* data);
        ~DataToFile()override;
        void update(size_t id);

};

