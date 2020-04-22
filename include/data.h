#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <fstream>
#include <csignal>
#include <algorithm>
#include <queue>
#include <typeinfo>
#include "logger.h"
#include <cstring>
#include <sstream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "pcout.h"


using Time = std::chrono::seconds;
using Bulk = std::pair<std::vector<std::string>,Time>;


class Observer {
public:
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
    Bulk getBulk();

    std::vector<std::thread*> vec_thread;
    std::queue<Bulk> bulkQ;
    std::condition_variable cv;
    std::mutex mtx_cmd;
    std::mutex mtx_file;

private:
    void clearData();
    void checkDilimiter(std::string& str);
    void setQueue();

    Subscrabers subs;
    std::pair<bool,uint8_t> checkD; ///< переменная для проверки использования знаков динамического разделения блоков "{" и "}" и хранения состояния о их кол-ве
    Bulk bulk;
    std::size_t count;        ///< хранит информацию о размере блока, задаеться при запуске программы (инициализируеться в конструкторе)
    std::size_t countTry;           ///< оставшееся ко-во команд для ввода в блок для его формирования
};

class DataToConsole:public Observer
{
    private:
    // std::shared_ptr<DataIn> _data;
    DataIn* _data;
    public:
        DataToConsole(DataIn* data);
        ~DataToConsole()override;
        void update(int id);
};

class DataToFile:public Observer
{
    private:
    // std::shared_ptr<DataIn> _data;
    DataIn* _data;
    public:
        DataToFile(DataIn* data);
        ~DataToFile()override;
        void update(int id);

};

