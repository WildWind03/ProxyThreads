#include <iostream>
#include "proxy_server.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    proxy_server proxy_server1(12000);
    proxy_server1.start();
    return 0;
}