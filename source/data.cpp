#include "data.h"


//-----Data input methods----------------------------------------------------------------------
    DataIn::DataIn(int count):count(count),countTry(count)
    {
        bulk = nullptr;
        works = true;
    }

    DataIn::~DataIn()
    {
        Writer{} << "DataIn - destructor-start" << std::endl;
        for(auto&i : vec_thread)
        {
            delete i;
        }
        Writer{} << "DataIn - destructor-middle" << std::endl;
        for(auto& i : subs)
        {
            delete i;
        }
        Writer{} << "DataIn - destructor-end" << std::endl;
        delete bulk;
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
                if(bulk->first.size())
                {
                    notify();
                    // bulk->first.clear();
                    delete bulk;
                    bulk = nullptr;
                    Writer{} << "checkDilimiter notify " << std::endl;
                }
                checkD.first = true;
                ++checkD.second;

            }
            
        }
        else if (str == "}")
        {
            if (checkD.second) --checkD.second;
            Writer{} << "checkDilimiter } " << std::endl;
        }
    }

    void DataIn::setData(std::string&& str) 
    {
        // std::scoped_lock sl{mtx_cmd,mtx_file};
        if(bulk == nullptr) bulk = new Bulk{};
        Logger::getInstance().set_lineCount(0);

        checkDilimiter(str);
        if(checkD.first)
        {
            if (str!="{" && str!="}")
            {
                if(bulk->first.size() == 0) 
                {
                    bulk->second = std::chrono::seconds(std::time(NULL));
                }
                Logger::getInstance().set_commandCount();
                bulk->first.emplace_back(str);
            }
            else if (!checkD.second)
            {
                write();
            }
        }
        else
        {
            if (str!="{" && str!="}" && countTry)
            {
                if(bulk->first.size() == 0)
                {
                    bulk->second = std::chrono::seconds(std::time(NULL));
                }
                Logger::getInstance().set_commandCount();
                bulk->first.emplace_back(str);
                --countTry;
            }
            if(!countTry)
            {
                write();
            }
        }
        
    }

    void DataIn::write()
    {
        notify();
        clearData();
    }

    void DataIn::notify() 
    {
        if(bulk)
        {
            Logger::getInstance().set_bulkCount();
            setQueues();
            cv.notify_all();
        }
        else cv.notify_all();
    }

    void DataIn::clearData()
    {   
        // bulk->first.clear();
        delete bulk;
        bulk = nullptr;
        checkD.first = false;
        checkD.second = 0;
        countTry = count;
    }

    void DataIn::setQueues()
    {
        for(auto& i : subs)
        {
            i->setBulk(*bulk);
        }
    }

//-----Data to console methods-------------------------------------------------------------------
    DataToConsole::DataToConsole(DataIn* data):_data(data)
    {
        data->subscribe(this);
    }

    DataToConsole::~DataToConsole()
    {
        Writer{} << "DataToConsole dest" << std::endl;
    }

    void DataToConsole::setBulk(const Bulk& bulk)
    {
        std::lock_guard<std::mutex> l{_data->mtx_cmd};
        Writer{} << "setBulk " << bulk.first.front() << std::endl;
        bulkQ.push(bulk);
    }

    void DataToConsole::update(size_t id)
    {
        // Writer{}<< "THREAD until cicle " << id << std::endl;
         while(_data->works || !bulkQ.empty())
        {
        // Writer{}<< "THREAD after cicle " << id << std::endl;
            std::unique_lock<std::mutex> consolLock(_data->mtx_cmd);
            _data->cv.wait(consolLock,[&](){
            if(!bulkQ.empty() || !_data->works) return true;
            else return false;
            });
        // Writer{}<< "THREAD into WAIT " << id << std::endl;

            while(!bulkQ.empty())
            {
         Writer{}<< "THREAD into while " << bulkQ.front().first.front() << std::endl;

                Logger::getInstance().set_bulkCount(id);
                Writer::console(bulkQ.front().first,id);
                bulkQ.pop();
            }
                
        }
    }

//-----Data to file methods-----------------------------------------------------------------------
    DataToFile::DataToFile(DataIn* data):_data(data)
    {
        data->subscribe(this);
    }

    DataToFile::~DataToFile()
    {
        Writer{} << "DataToFile dest" << std::endl;
    }

    void DataToFile::setBulk(const Bulk& bulk)
    {
        std::lock_guard<std::mutex> l{_data->mtx_file};
        bulkQ.push(bulk);
    }

    void DataToFile::update(size_t id)
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
                    Logger::getInstance().set_bulkCount(id);
                    Writer::file(bulkQ.front(),id,start);
                    bulkQ.pop();
                }
        }
    }
