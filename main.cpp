#pragma once

#include <iostream>
#include "proxy_server.h"

int main() {
    try {
        proxy_server proxy_server1(12000);
        proxy_server1.start();
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}