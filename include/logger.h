#pragma once
#include <map>
#include <array>
#include <iostream>


class Logger
{
    private:
    std::map <const int,std::array<int,3>> m;
    Logger(){}

    public:
    static Logger& getInstance()
    {
        static Logger logger;
        return logger;
    }

    void set_lineCount(int id = 0)
    {
        ++(m[id][0]);
    }

    void set_commandCount(int id = 0)
    {
        ++(m[id][1]);
    }   

    void set_bulkCount(int id = 0)
    {
        ++(m[id][2]);
    } 


    void print()
    {
        for (size_t i = 0; i < m.size();++i)
        {
            if (i == 0)
            {
                std::cout << "Main thread: "    << m[0][0]<< "- lines," 
                                        << m[0][1]<< "- commands,"
                                        << m[0][2]<< "- bulks" << std::endl;
            }
            else
            {
                std::cout << "Thread # " << i <<": "
                                        << m[i][1]<< "- commands,"
                                        << m[i][2]<< "- bulks" << std::endl;
            }
            
        }
     }

     ~Logger()
     {
     }
};