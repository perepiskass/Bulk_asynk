#include "myasync.h"
#include "data.h"
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>
#include <future>

namespace async 
{

void setCommands(DataIn* _handle,char* &&copy)
{
    std::lock_guard<std::mutex> lg(_handle->mtx_input);
    const char* delim = "\n";
    auto ptr = std::strtok(copy,delim);
    while (ptr)
    {
        _handle->setData(ptr);
        ptr = strtok(0,delim);
    }
}

handle_t connect(std::size_t bulk) 
{
    static size_t i = 0;
    auto bulkPtr = new DataIn(bulk);
    auto cmdPtr = new DataToConsole(bulkPtr);
    auto filePtr = new DataToFile(bulkPtr);
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([cmdPtr](){cmdPtr->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i);}));
    

    Logger::getInstance().setCount(i);

    return reinterpret_cast<void*>(bulkPtr);
}

void receive(handle_t handle,const char *data,std::size_t size) 
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    char* copy =(char*) std::malloc(size);
    std::strcpy(copy,data);
    auto as(std::async(std::launch::async,setCommands, _handle, std::move(copy)));
}

void disconnect(handle_t handle) 
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    _handle->works = false;
    _handle->write();
    for(auto& i : _handle->vec_thread)
    {
        i->join();
    }
    delete _handle;
}

}
namespace logger
{
    void printLog()
    {
        Logger::getInstance().print();
    }

}