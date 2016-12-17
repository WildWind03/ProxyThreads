//
// Created by alexander on 09.12.16.
//

#ifndef PROXY_OBSERVABLE_H
#define PROXY_OBSERVABLE_H

#include <map>
#include "my_observer.h"

class observable {

protected:
    std::map<int, observer*> observers;

public:
    void notify(int event) {
        for (auto & iter : observers) {
            iter.second -> update(event);
        }
    }

    void notify(int fd, int event_type1) {
        observers.find(fd).operator*().second->update(event_type1);
    }

    virtual void add_new_observer(observer *observer1, int key) {
        observers.insert(std::pair<int, observer*> (key, observer1));
    }

    virtual void delete_observer(int key) {
        observers.erase(key);
    }

    std::map<int, observer*> get_observers() {
        return observers;
    };
};

#endif //PROXY_OBSERVABLE_H
