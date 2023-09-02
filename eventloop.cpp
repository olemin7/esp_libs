/*
 * ceventloop.cpp
 *
 *  Created on: Oct 19, 2022
 *      Author: oleksandr
 */

#include "eventloop.h"

#include <algorithm>
#include <list>
#include <logs.h>

namespace event_loop {
using std::chrono::steady_clock;
using namespace std::chrono_literals;

std::unique_ptr<std::list<pevent>> list_ptr;

class ctimeout: public ievent {
 private:
    virtual void rearm() override {
        is_actual_ = false;
    }

 public:
    ctimeout(std::function<void()>&& handler, steady_clock::duration timeout)
        : ievent(std::move(handler), steady_clock::now() + timeout) {}
};

class cinterval: public ievent {
 private:
    std::chrono::steady_clock::duration period_;
    virtual void                        rearm() override {
        when_ = steady_clock::now() + period_;
    }

 public:
    cinterval(std::function<void()>&& handler, std::chrono::steady_clock::duration period, bool call_on_start)
        : ievent(std::move(handler), call_on_start ? steady_clock::now() : (steady_clock::now() + period))
        , period_(period) {}
};

pevent set_timeout(std::function<void()>&& handler, std::chrono::steady_clock::duration timeout) {
    if (list_ptr) {
        DBG_OUT << "set_timeout =" << std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count() << "ms"
                << std::endl;
        list_ptr->emplace_back(std::make_shared<ctimeout>(std::move(handler), timeout));
        return list_ptr->back();
    } else {
        DBG_OUT << "is not inited" << std::endl;
        return {};
    }
}

pevent set_interval(std::function<void()>&& handler, std::chrono::steady_clock::duration period, bool call_on_start) {
    if (list_ptr) {
        DBG_OUT << "set_interval =" << std::chrono::duration_cast<std::chrono::milliseconds>(period).count() << "ms"
                << ", call_on_start=" << call_on_start << std::endl;
        list_ptr->emplace_back(std::make_shared<cinterval>(std::move(handler), period, call_on_start));
        return list_ptr->back();
    } else {
        DBG_OUT << "is not inited" << std::endl;
        return {};
    }
}

void init() {
    list_ptr = std::make_unique<std::list<pevent>>();
}

void loop() {
    if (list_ptr) {
        const auto                                   now  = steady_clock::now();
        static std::chrono::steady_clock::time_point next = {};
        if (now > next) {
            next = now + 10ms;
            list_ptr->remove_if([](const auto& el) -> bool { return el && (el->get_isActual() == false); });

            for (auto& el : *list_ptr) {
                if (el && el->get_isActual() && (now >= el->get_when())) { // lunch one by loop
                    el->trigger();
                }
            }
        }
    } else {
        DBG_OUT << "is not inited" << std::endl;
    }
}

} // namespace event_loop
