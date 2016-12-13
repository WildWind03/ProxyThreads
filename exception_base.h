//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_EXCEPTION_CAN_NOT_START_PROXY_H
#define PROXYTHREADS_EXCEPTION_CAN_NOT_START_PROXY_H

#include <exception>
#include <string>

class exception_base : public std::exception {
    std::string text;

public:
    exception_base(std::string exception_text) {
        this -> text = exception_text;
    }

    std::string get_text() {
        return text;
    }

};
#endif //PROXYTHREADS_EXCEPTION_CAN_NOT_START_PROXY_H
