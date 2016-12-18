//
// Created by alexander on 17.12.16.
//
#pragma once
#ifndef PROXYTHREADS_EVENTS_H
#define PROXYTHREADS_EVENTS_H
class events {

public:
    static const int REQUEST_GOT = 2;
    static const int STREAM_ENTRY = 3;
    static const int SEND_TO_BROWSER_ERROR = 4;
    static const int DELETE_REQUEST = 5;
    static const int DELETE_ENTRY_FROM_CACHE = 7;
    events() = delete;
};
#endif //PROXYTHREADS_EVENTS_H
