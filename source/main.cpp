#include "version_lib.h"
#include "myasync.h"
#include <iostream>
#include "logger.h"

void checkArg(int argc,char** argv);

int main(int argc, char *argv[]) 
{
  try
  {
    checkArg(argc,argv);

    std::size_t bulk = 5;
    std::size_t bulk2 = 1;

    auto h = async::connect(bulk);
    auto h2 = async::connect(bulk2);
    auto h3 = async::connect(6);

    async::receive(h, "1", 1);
    async::receive(h2, "A\n", 2);
    async::receive(h, "\n2\n3\n4\n5\nDOTNWORK-1h\n{\na\n", 25);
    async::receive(h, "b\nc\nd\n}\nDOTNWORK-2h", 19);
    async::receive(h3, "\nC\nC\nC\nC\nC\n{\nX\n", 15);

    async::disconnect(h);
    async::receive(h2, "S\n", 2);
    async::disconnect(h2);

    async::receive(h3, "\n1\n2\n3\n4\n5\nZ\n}\n", 15);

    async::disconnect(h3);
    std::cout << std::endl;
    Logger::getInstance().print();
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
    return 0;
}
