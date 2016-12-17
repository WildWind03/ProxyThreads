//
// Created by alexander on 17.12.16.
//

#ifndef PROXYTHREADS_EXCEPTION_INVALID_HTTP_H
#define PROXYTHREADS_EXCEPTION_INVALID_HTTP_H

#include <string>
#include <exception>
#include "exception_base.h"

class exception_invalid_http_data : public exception_base {

public:
    exception_invalid_http_data(std::string string) : exception_base(string) {
    }
};
#endif //PROXYTHREADS_EXCEPTION_INVALID_HTTP_H
