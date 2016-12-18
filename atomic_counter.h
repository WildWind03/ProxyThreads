//
// Created by alexander on 18.12.16.
//

#ifndef PROXYTHREADS_ATOMIC_COUNTER_H
#define PROXYTHREADS_ATOMIC_COUNTER_H

#include <zconf.h>
#include <pthread.h>

class atomic_counter {
    int value;
    pthread_mutex_t pthread_mutex;

public:
    atomic_counter(int start_value) {
        this -> value = start_value;
        pthread_mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    void increment() {
        pthread_mutex_lock(&pthread_mutex);
        ++value;
        pthread_mutex_unlock(&pthread_mutex);
    }

    void decrement() {
        pthread_mutex_lock(&pthread_mutex);
        --value;
        pthread_mutex_unlock(&pthread_mutex);
    }

    void lock() {
        pthread_mutex_lock(&pthread_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&pthread_mutex);
    }

    int get() {
        return value;
    }

    void set_value(int new_value) {
        pthread_mutex_lock(&pthread_mutex);
        value = new_value;
        pthread_mutex_unlock(&pthread_mutex);
    }

    virtual ~atomic_counter() {
        pthread_mutex_destroy(&pthread_mutex);
    }
};
#endif //PROXYTHREADS_ATOMIC_COUNTER_H
