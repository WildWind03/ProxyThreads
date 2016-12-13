//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_PROXY_SERVER_H
#define PROXYTHREADS_PROXY_SERVER_H


class proxy_server {
    int socket_fd;
public:
    proxy_server(int port);
    void start();
    virtual ~proxy_server();
};


#endif //PROXYTHREADS_PROXY_SERVER_H
