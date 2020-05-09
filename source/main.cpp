#include "version_lib.h"
#include "myasync.h"
#include <iostream>
// #include "logger.h"

void checkArg(int argc,char** argv);

int main(int argc, char *argv[]) 
{
  try
  {
    checkArg(argc,argv);

    std::size_t bulk1 = 5;
    std::size_t bulk2 = 1;
    std::size_t bulk3 = 6;

    auto h1 = async::connect(bulk1);
    auto h2 = async::connect(bulk2);
    auto h3 = async::connect(bulk3);

    async::receive(h1, "1", 1);
    async::receive(h2, "A\n", 2);
    async::receive(h1, "\n2\n3\n4\n5\nDONtWORK-1h\n{\n6\n", 25);
    async::receive(h1, "7\n8\n9\n}\nDONtWORK-2h", 19);
    async::receive(h3, "\nM\nA\nZ\nD\nA\n{\nC\n", 15);

    async::disconnect(h1); 

    async::receive(h2, "Z\n", 2);
    async::disconnect(h2);

    async::receive(h3, "\nI\nT\nR\nO\nE\nN\n}\n", 15);
    async::disconnect(h3);

    std::cout << std::endl;
    logger::printLog();
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
    return 0;
}
