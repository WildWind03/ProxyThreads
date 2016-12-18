//
// Created by alexander on 17.12.16.
//
#pragma once
#ifndef PROXYTHREADS_EVENTS_H
#define PROXYTHREADS_EVENTS_H
class events {

public:
    static const int DELETE_REQUEST = 1;
    static const int REQUEST_GOT = 2;
    static const int CONNECTION_WITH_SERVER_FAILED = 3;
    static const int NEW_DATA = 4;
    static const int CAN_WRITE = 5;
    events() = delete;
};
#endif //PROXYTHREADS_EVENTS_H
