//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_LOGGER_H
#define PROXYTHREADS_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>

class logger {
    std::string name;
    std::string filename;

public:
    logger(std::string name, std::string filename)  {
        this -> name = "[" + name + "] ";
        this -> filename = filename;
    }

    void log(std::string string) {
        std::ofstream ofstream1(filename, std::ios_base::app);
        ofstream1 << name << string << std::endl;
        ofstream1.close();
    }
};
#endif //PROXYTHREADS_LOGGER_H
