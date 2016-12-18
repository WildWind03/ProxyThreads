//
// Created by alexander on 18.12.16.
//

#ifndef PROXYTHREADS_CACHE_ENTRY_H
#define PROXYTHREADS_CACHE_ENTRY_H

#include <cstring>
#include "my_observer.h"
#include "concurrent_hash_map.h"

class cache_entry {
    char* data;
    size_t current_length;

    concurrent_hash_map<int, observer*> readers;

public:
    cache_entry() {

    }

    void add_reader(int fd, observer *observer1) {
        readers.insert(fd, observer1);
    }

};
#endif //PROXYTHREADS_CACHE_ENTRY_H
