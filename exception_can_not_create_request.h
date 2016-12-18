//
// Created by alexander on 13.12.16.
//
#pragma once
#ifndef PROXYTHREADS_EXCEPTION_CAN_NOT_CREATE_REQUEST_H
#define PROXYTHREADS_EXCEPTION_CAN_NOT_CREATE_REQUEST_H

#include <exception>
#include "exception_base.h"

class exception_can_not_create_request : public exception_base {
public:
    exception_can_not_create_request(const std::string &exception_text) : exception_base(exception_text) {}

};
#endif //PROXYTHREADS_EXCEPTION_CAN_NOT_CREATE_REQUEST_H
