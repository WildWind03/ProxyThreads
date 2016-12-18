//
// Created by alexander on 13.12.16.
//
#pragma once
#ifndef PROXYTHREADS_REQUEST_SERVER_H
#define PROXYTHREADS_REQUEST_SERVER_H

#include "request_base.h"

class request_server : public request_base {
public:
    request_server(int socket_fd, sockaddr_in addr) : request_base(socket_fd, addr) {

    }

};
#endif //PROXYTHREADS_REQUEST_SERVER_H
