#include "data.h"
#include "pcout.h"
static bool cmd_run;


//-----Data input methods----------------------------------------------------------------------
    DataIn::DataIn(int count):count(count),countTry(count)
    {}
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
                bulk.first.clear();
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
            }
        }
        
    }

    void DataIn::notify() 
    {
        Logger::getInstance().set_bulkCount();

        setQueue();

        cmd_run = true;

        cv.notify_all();

        clearData();
    }

    void DataIn::clearData()
    {   
        while (cmd_run)
        {
            continue;
        }

        bulk.first.clear();
        checkD.first = false;
        checkD.second = 0;
        countTry = count;
    }


    Bulk DataIn::getBulk()
    {
        return bulk;
    }

    void DataIn::setQueue()
    {
        std::lock_guard<std::mutex> lg(mtx_file);
        bulkQ.push(bulk);
    }


//-----Data to console methods-------------------------------------------------------------------
    DataToConsole::DataToConsole(DataIn* data):_data(data)
    {
        data->subscribe(this);
    }
    DataToConsole::~DataToConsole()
    {
        std::cout << "DataToConsole dest" << std::endl;
    }

    void DataToConsole::update(int id)
    {
         while(true)
        {
            std::unique_lock<std::mutex> consolLock(_data->mtx_cmd);
            while(!cmd_run) _data->cv.wait(consolLock);

                auto bulk = _data->getBulk();
                // cmd_run = false;
                // consolLock.unlock();

                if(bulk.first.size())
                { 
                    Logger::getInstance().set_bulkCount(id);
                    std::cout << "bulk " << id << ": ";
                    for(auto str = bulk.first.begin(); str!=bulk.first.end(); ++str)
                    {
                        Logger::getInstance().set_commandCount(id);
                        if(str==bulk.first.begin()) std::cout << *str;
                        else std::cout << ", " << *str;
                    }
                    std::cout << std::endl;
                }
                cmd_run = false;
        }
    }



//-----Data to file methods-----------------------------------------------------------------------
    DataToFile::DataToFile(DataIn* data):_data(data)
    {
        data->subscribe(this);
    }
    DataToFile::~DataToFile()
    {
        std::cout << "DataToFile dest" << std::endl;
    }


    void DataToFile::update(int id)
    {
        while (true)
        {
                std::unique_lock<std::mutex> fileLock(_data->mtx_file);
                _data->cv.wait(fileLock,[&](){return !_data->bulkQ.empty();});
                    while (!_data->bulkQ.empty())
                    {
                                
                        auto start(std::chrono::steady_clock::now());
        
                        auto bulk = _data->bulkQ.front();
                        _data->bulkQ.pop();
                        // fileLock.unlock();  
                    if(bulk.first.size())
                    {
                        Logger::getInstance().set_bulkCount(id);
                        std::ofstream out;
                        auto timeUNIX = bulk.second.count();
                        auto end(std::chrono::steady_clock::now());
                            // using seconds = std::chrono::duration<double>;
                            // using nanoseconds = std::chrono::duration<double, std::ratio_multiply<seconds::period,std::nano>>;
                        using nanoseconds = std::chrono::duration<double,std::ratio<1,1'000'000'000>>;
                        auto diff = nanoseconds(end - start).count();
                        std::string path = "bulk"+ std::to_string(timeUNIX) + '.' + std::to_string(int(diff)) + ".log";
                        out.open(path);
                        if (out.is_open(),std::ios::app)
                        {
                            out << "bulk: "    << id << ' ';
                            for(auto str = bulk.first.begin(); str!=bulk.first.end(); ++str)
                            {
                                Logger::getInstance().set_commandCount(id);

                                if(str==bulk.first.begin()) out << *str;
                                else out << ", " << *str;
                            }
                        }
                        out.close();
                    }
                    }
        }
    }
