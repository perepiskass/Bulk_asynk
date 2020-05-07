#include "myasync.h"
#include "data.h"
#include <cstdlib>
#include <thread>
#include <chrono>
#include <future>

namespace async 
{

void setCommands(DataIn* _handle,char* copy,const char* delim)
{
    std::lock_guard<std::mutex> lg(_handle->mtx_input);
    auto ptr = std::strtok(copy,delim);
    while (ptr)
    {
        _handle->setData(ptr);
        ptr = strtok(0,delim);
    }
}

handle_t connect(std::size_t bulk) 
{
    static size_t i = 1;
    auto bulkPtr = new DataIn(bulk);
    auto cmdPtr = new DataToConsole(bulkPtr);
    auto filePtr = new DataToFile(bulkPtr);

    bulkPtr->vec_thread.emplace_back(new std::thread ([cmdPtr](){cmdPtr->update(i++);}));
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i++);}));
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i++);}));

    return reinterpret_cast<void*>(bulkPtr);
}

void receive(handle_t handle,const char *data,std::size_t size) 
{
    // Writer{} << "receive start " << size << std::endl;
    auto _handle = reinterpret_cast<DataIn*>(handle);

    const char* delim = "\n";
    
    char* copy =(char*) std::malloc(size);
    std::strcpy(copy,data);
    
    // setCommands(_handle,copy,delim);
    auto as(std::async(std::launch::async,setCommands, _handle, copy, delim));
    
    // Writer{} << "receive end " << size << std::endl;
}

void disconnect(handle_t handle) 
{
    // Writer{} << "disconnect start " << std::endl;
    auto _handle = reinterpret_cast<DataIn*>(handle);
    _handle->works = false;
    _handle->threadStart();
    // Writer{} << "disconnect middle " << std::endl;
    for(auto& i : _handle->vec_thread)
    {
    // Writer{} << "until join " << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // i->detach();
        i->join();
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Writer{} << "after join " << std::endl;

    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    delete _handle;
}

}
