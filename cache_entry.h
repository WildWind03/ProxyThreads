//
// Created by alexander on 18.12.16.
//

#ifndef PROXYTHREADS_CACHE_ENTRY_H
#define PROXYTHREADS_CACHE_ENTRY_H

#include <cstring>
#include "my_observer.h"
#include "concurrent_hash_map.h"
#include "events.h"

class cache_entry {
    char* data;
    size_t current_length = 0;
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
    size_t min_reader_pos = 0;

    concurrent_hash_map<int, observer*> readers;

    observer *writer;
    bool is_finished = false;

    const size_t MAX_DATA_SIZE = 5 * 1024 * 1024;

public:
    cache_entry() {
        data = new char[MAX_DATA_SIZE];
    }

    char* get_char_to_read(size_t current_pos) {
        pthread_rwlock_rdlock(&rwlock);
        return data + current_pos;
    }

    void update_data_was_read(size_t current_pos) {


        pthread_rwlock_unlock(&rwlock);

        if (!is_finished) {

        }
    }

    void update_data_was_add(size_t count_of_added_bytes) {
        current_length += count_of_added_bytes;
        pthread_rwlock_unlock(&rwlock);

        readers.lock_read();

        for (auto iter : readers) {
            iter.second->update(events::NEW_DATA, nullptr);
        }

        readers.unlock();
    }

    char* get_char_to_write(size_t current_pos) {
        pthread_rwlock_wrlock(&rwlock);
        return data + current_pos;
    }

    void add_reader(int fd, observer *observer1) {
        readers.insert(fd, observer1);
    }

    void delete_reader(int fd) {
        readers.erase(fd);
    }

    void mark_finished() {
        is_finished = true;
    }

    virtual ~cache_entry() {
        pthread_rwlock_destroy(&rwlock);
        delete data;
    }

};
#endif //PROXYTHREADS_CACHE_ENTRY_H
