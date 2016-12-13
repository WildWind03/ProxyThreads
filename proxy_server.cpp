//
// Created by alexander on 13.12.16.
//

#include <stdexcept>
#include <bits/socket_type.h>
#include <bits/socket.h>
#include <sys/socket.h>
#include "proxy_server.h"
#include "exception_base.h"
#include "exception_can_not_start_server.h"

proxy_server::proxy_server(int port) {
    if (port <= 0) {
        throw exception_can_not_start_server ("Typed port (" + std::to_string(port) + ") is less than zero. The port must be positive");
    }

    socket_fd = socket (AF_INET, SOCK_STREAM, 0);

    if (-1 == socket_fd) {
        throw exception_can_not_start_server("");
    }
}

void proxy_server::start() {

}

proxy_server::~proxy_server() {

}
