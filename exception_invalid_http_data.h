//
// Created by alexander on 08.12.16.
//

#ifndef PROXY_HTTP_PARSE_EXCEPTION_H
#define PROXY_HTTP_PARSE_EXCEPTION_H

#include <string>
#include <exception>

class exception_invalid_http_data : public exception_base {

public:
    exception_invalid_http_data(std::string string) : exception_base(string) {
    }
};
#endif //PROXY_HTTP_PARSE_EXCEPTION_H
