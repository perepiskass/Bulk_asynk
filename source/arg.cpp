#include "version_lib.h"
#include <iostream>
#include <string>

/**
 * @brief Функция для проверки введенных аргументов программы.
 */
void checkArg(int argc,char** argv)
{

    if ( argc > 1)
    {
        std::string version = "--version";
        if (argv[1] == version)
        {
            std::cout << "version: " << version_major()<< '.'<< version_minor() << '.' << version_patch() << std::endl;
            exit(0);
        }
        else
        {
            std::cout << "Enter --version or nothing" << std::endl;
            exit(0);
        }
    }
    
}