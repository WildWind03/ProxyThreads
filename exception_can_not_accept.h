//
// Created by alexander on 13.12.16.
//
#pragma once
#ifndef PROXYTHREADS_EXCEPTION_CAN_NOT_ACCEPT_H
#define PROXYTHREADS_EXCEPTION_CAN_NOT_ACCEPT_H

#include "exception_base.h"

class exception_can_not_accept : public exception_base {
public:
    exception_can_not_accept(const std::string &exception_text) : exception_base(exception_text) {}

};
#endif //PROXYTHREADS_EXCEPTION_CAN_NOT_ACCEPT_H
