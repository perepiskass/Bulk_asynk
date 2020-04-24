#include "data.h"


//-----Data input methods----------------------------------------------------------------------
    DataIn::DataIn(int count):count(count),countTry(count)
    {
        works = true;
    }
    DataIn::~DataIn()
    {
        // this->notify();
        std::cout << "DataIn - destructor-1" << std::endl;
        for(auto&i : subs)
        {
            delete i;
        }
        std::cout << "DataIn - destructor-2" << std::endl;
        for(auto& i : vec_thread)
        {
            delete i;
        }
        std::cout << "DataIn - destructor-3" << std::endl;
    }

    void DataIn::subscribe(Observer *obs)
    {
        subs.push_back(obs);
    }

    void DataIn::checkDilimiter(std::string& str)
    {

        if (str == "{")
        {
            if(checkD.first) ++checkD.second;
            else
            {
                if(bulk.first.size())
                {
                    notify();
                    bulk.first.clear();
                }
                checkD.first = true;
                ++checkD.second;
            }
            
        }
        else if (str == "}")
        {
            if (checkD.second) --checkD.second;
        }
    }

    void DataIn::setData(std::string&& str) 
    {
        Logger::getInstance().set_lineCount(0);

        checkDilimiter(str);
        if(checkD.first)
        {
            if (str!="{" && str!="}")
            {
                if(bulk.first.size() == 0) 
                {
                    bulk.second = std::chrono::seconds(std::time(NULL));
                }
                Logger::getInstance().set_commandCount();
                bulk.first.emplace_back(str);
            }
            else if (!checkD.second)
            {
                notify();
                clearData();
            }
        }
        else
        {
            if (str!="{" && str!="}" && countTry)
            {
                if(bulk.first.size() == 0)
                {
                    bulk.second = std::chrono::seconds(std::time(NULL));
                }
                Logger::getInstance().set_commandCount();
                bulk.first.emplace_back(str);
                --countTry;
            }
            if(!countTry)
            {
                notify();
                clearData();
            }
        }
        
    }

    void DataIn::notify() 
    {
        Logger::getInstance().set_bulkCount();

        setQueues();
        cv.notify_all();
    }

    void DataIn::clearData()
    {   
        bulk.first.clear();
        checkD.first = false;
        checkD.second = 0;
        countTry = count;
    }

    void DataIn::setQueues()
    {
        std::scoped_lock lck{mtx_cmd, mtx_file};
        for(auto& i : subs)
        {
            i->setBulk(bulk);
        }
    }


//-----Data to console methods-------------------------------------------------------------------
    DataToConsole::DataToConsole(std::shared_ptr<DataIn> data):_data(data)
    {
        data->subscribe(this);
    }

    DataToConsole::~DataToConsole()
    {
        std::cout << "DataToConsole dest" << std::endl;
    }

    void DataToConsole::setBulk(const Bulk& bulk)
    {
        bulkQ.push(bulk);
    }

    void DataToConsole::update(int id)
    {
         while(_data->works || !bulkQ.empty())
        {
            std::unique_lock<std::mutex> consolLock(_data->mtx_cmd);
            _data->cv.wait(consolLock,[&](){
            if(!bulkQ.empty() || !_data->works) return true;
            else return false;
            });
            while(!bulkQ.empty())
            {
                if(bulkQ.front().first.size())
                { 
                    Logger::getInstance().set_bulkCount(id);
                    Writer::Console(bulkQ.front().first,id);
                }
                bulkQ.pop();
            }
                
        }
    }



//-----Data to file methods-----------------------------------------------------------------------
    DataToFile::DataToFile(std::shared_ptr<DataIn> data):_data(data)
    {
        data->subscribe(this);
    }

    DataToFile::~DataToFile()
    {
        std::cout << "DataToFile dest" << std::endl;
    }

    void DataToFile::setBulk(const Bulk& bulk)
    {
        bulkQ.push(bulk);
    }

    void DataToFile::update(int id)
    {
        while (_data->works || !bulkQ.empty())
        {
            std::unique_lock<std::mutex> fileLock(_data->mtx_file);
            _data->cv.wait(fileLock,[&](){
            if(!bulkQ.empty() || !_data->works) return true;
            else return false;
            });
                while (!bulkQ.empty())
                {
                    auto start(std::chrono::steady_clock::now());
                    if(bulkQ.front().first.size())
                    {
                        Logger::getInstance().set_bulkCount(id);
                        Writer::File(bulkQ.front(),id,start);
                    }
                    bulkQ.pop();
                }
        }
    }
