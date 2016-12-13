//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_EXCEPTION_CAN_NOT_START_SERVER_H
#define PROXYTHREADS_EXCEPTION_CAN_NOT_START_SERVER_H

#include "exception_base.h"

class exception_can_not_start_server : public exception_base {
public:
    exception_can_not_start_server(const std::string &exception_text) : exception_base(exception_text) {

    }

};
#endif //PROXYTHREADS_EXCEPTION_CAN_NOT_START_SERVER_H
