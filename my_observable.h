//
// Created by alexander on 09.12.16.
//
#pragma once

#ifndef PROXY_OBSERVABLE_H
#define PROXY_OBSERVABLE_H

#include <map>
#include "my_observer.h"
#include "concurrent_hash_map.h"

class observable {

protected:
    concurrent_hash_map<int, observer*> observers;

public:
    void notify(int event, void *data) {
        observers.lock_write();

        for (auto & iter : observers) {
            iter.second -> update(event, data);
        }

        observers.unlock();
    }

    void notify(int fd, int event_type1, void *data) {
        observers.lock_write();
        observers.find(fd).operator*().second->update(event_type1, data);
        observers.unlock();
    }

    virtual void add_new_observer(observer *observer1, int key) {
        observers.insert(key, observer1);
    }

    virtual void delete_observer(int key) {
        observers.erase(key);
    }
};

#endif //PROXY_OBSERVABLE_H
