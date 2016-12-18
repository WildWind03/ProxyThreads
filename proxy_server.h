//
// Created by alexander on 13.12.16.
//
#pragma once
#ifndef PROXYTHREADS_PROXY_SERVER_H
#define PROXYTHREADS_PROXY_SERVER_H

#include <map>
#include "request_base.h"
#include "my_observer.h"
#include "concurrent_hash_map.h"
#include "cache_entry.h"

class proxy_server : public observer {
    const char *BIND_IP = "127.0.0.1";
    const int MAX_COUNT_OF_PENDING_REQUESTS = 100;

    int socket_fd;
    bool is_running = true;
    //concurrent_hash_map <int, request_base*> requests;
    concurrent_hash_map <std::string, cache_entry*> cache;

public:

    proxy_server(int port);
    void start();
    void stop();
    virtual ~proxy_server();

    virtual void update(int event_type1, void *data) override;
};


#endif //PROXYTHREADS_PROXY_SERVER_H
