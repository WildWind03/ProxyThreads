//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_SOCKETS_UTIL_H
#define PROXYTHREADS_SOCKETS_UTIL_H

#include <bits/time.h>
#include <sys/socket.h>

class sockets_utl {
    sockets_utl() = delete;

    static int set_socket_timeout(int socket_fd, int seconds) {
        struct timeval timeout;
        timeout.tv_sec = seconds;
        timeout.tv_usec = 0;

        return setsockopt (socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                        sizeof(timeout));
    }
};
#endif //PROXYTHREADS_SOCKETS_UTIL_H
