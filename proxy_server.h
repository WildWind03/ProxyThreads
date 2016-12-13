//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_PROXY_SERVER_H
#define PROXYTHREADS_PROXY_SERVER_H

#include <map>
#include "request_base.h"

class proxy_server {
    const char *BIND_IP = "127.0.0.1";
    const int MAX_COUNT_OF_PENDING_REQUESTS = 100;

    int socket_fd;
    bool is_running;
    std::map <int, request_base*> requests;

public:
    proxy_server(int port);
    void start();
    void stop();
    virtual ~proxy_server();
};


#endif //PROXYTHREADS_PROXY_SERVER_H
