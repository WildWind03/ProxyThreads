//
// Created by alexander on 18.12.16.
//

#ifndef PROXYTHREADS_CACHE_ENTRY_H
#define PROXYTHREADS_CACHE_ENTRY_H

#include <cstring>
#include "my_observer.h"
#include "concurrent_hash_map.h"
#include "events.h"
#include "atomic_counter.h"

class cache_entry {
    char* data;
    size_t current_length = 0;
    pthread_mutex_t mutex;
    pthread_cond_t cond_reader;
    pthread_cond_t cond_writer;

    atomic_counter atomic_counter1 = 0;

    concurrent_hash_map<int, observer*> readers;

    bool is_finished = false;

    const size_t MAX_DATA_SIZE = 5 * 1024 * 1024;

public:
    cache_entry() {
        data = new char[MAX_DATA_SIZE];
        mutex = PTHREAD_MUTEX_INITIALIZER;
        cond_reader = PTHREAD_COND_INITIALIZER;
        cond_writer = PTHREAD_COND_INITIALIZER;
    }

    int read(int socket_fd, char* src) {
        size_t pos = 0;
        int result;

        while(true) {
            pthread_mutex_lock(&mutex);

            if (pos == current_length) {
                atomic_counter1.increment();

                readers.lock_read();

                if (atomic_counter1.get() == readers.size()) {
                    pthread_cond_signal(&cond_writer);
                }
            }

            while (pos == current_length) {
                pthread_cond_wait(&cond_reader, &mutex);
            }

            ssize_t count_of_sent_chars = send(socket_fd, data + pos, current_length - pos, MSG_NOSIGNAL);

            if (-1 == count_of_sent_chars) {
                result = -1;
                break;
            }

            pos += count_of_sent_chars;

            pthread_mutex_unlock(&mutex);
        }
    }

    int write(int socket_fd, char* dest) {
        int result;

        pthread_mutex_lock(&mutex);

        while(true) {
            while (0 == MAX_DATA_SIZE - current_length) {
                pthread_cond_wait(&cond_writer, &mutex);
            }

            if (0 == MAX_DATA_SIZE - current_length) {
                if (atomic_counter1.get() == readers.size()) {
                    atomic_counter1.set_value(0);
                    current_length = 0;
                    readers.unlock();
                }
            }

            ssize_t count_of_received_bytes = recv(socket_fd, dest + current_length,
                                                   MAX_DATA_SIZE - current_length, 0);

            current_length += count_of_received_bytes;

            if (-1 == count_of_received_bytes) {
                result = -1;
                break;
            }

            if (0 == count_of_received_bytes) {
                is_finished = true;
                result = 0;
                break;
            }

            pthread_cond_broadcast(&cond_reader);
        }

        pthread_mutex_unlock(&mutex);

        return result;
    }

   /* char* get_char_to_read(size_t current_pos) {
        //pthread_rwlock_rdlock(&rwlock);
        return data + current_pos;
    }

    void update_data_was_read(size_t current_pos) {
        if (current_pos == current_length) {

        }

        if (current_pos == MAX_DATA_SIZE) {
            atomic_counter1.increment();
        }

        if (!is_finished && atomic_counter1.get() == readers.size()) {
            writer->update(events::CAN_WRITE, nullptr);
        }

        //pthread_rwlock_unlock(&rwlock);
    }

    void update_data_was_add(size_t count_of_added_bytes) {
        current_length += count_of_added_bytes;
        //pthread_rwlock_unlock(&rwlock);

        readers.lock_read();

        for (auto iter : readers) {
            iter.second->update(events::NEW_DATA, nullptr);
        }

        readers.unlock();
    }

    char* get_char_to_write(size_t current_pos) {
        //pthread_rwlock_wrlock(&rwlock);
        return data + current_pos;
    }
    */

    void add_reader(int fd, observer *observer1) {
        readers.insert(fd, observer1);
    }

    void delete_reader(int fd) {
        readers.erase(fd);
    }

    virtual ~cache_entry() {
        //pthread_rwlock_destroy(&rwlock);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_reader);
        pthread_cond_destroy(&cond_writer);
        delete data;
    }

};
#endif //PROXYTHREADS_CACHE_ENTRY_H
