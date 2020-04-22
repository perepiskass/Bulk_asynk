#include "myasync.h"
#include "data.h"
#include <cstdlib>

namespace async 
{

handle_t connect(std::size_t bulk) 
{
    static int i = 1;
    auto bulkPtr = new DataIn(bulk);
    auto cmdPtr = new DataToConsole(bulkPtr);
    auto filePtr = new DataToFile(bulkPtr);

    bulkPtr->vec_thread.emplace_back(new std::thread ([cmdPtr](){cmdPtr->update(i++);}));
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i++);}));
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i++);}));

    // constexpr size_t count_thread = 4;    ///< общее колличество потоков в программе.
    // Logger::getInstance().init(count_thread);


    for(auto& i : bulkPtr->vec_thread)
    {
        i->detach();
    }

    return reinterpret_cast<void*>(bulkPtr);
}

void receive(handle_t handle,const char *data,std::size_t size) 
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    const char* delim = "\n";
    
    char* copy =(char*) std::malloc(size);
    std::strcpy(copy,data);
    auto ptr = std::strtok(copy,delim);
    while (ptr)
    {
        _handle->setData(ptr);
        ptr = strtok(0,delim);
    }
}

void disconnect(handle_t handle) 
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    _handle->notify();
}

}
