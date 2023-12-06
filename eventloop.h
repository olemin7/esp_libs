/*
 * ceventloop.h
 *
 *  Created on: Oct 19, 2022
 *      Author: oleksandr
 */

#pragma once
#include <chrono>
#include <functional>
#include <memory>
#include <stdint.h>

namespace event_loop {
class ievent {
 protected:
    std::chrono::steady_clock::time_point when_;
    bool                                  is_actual_ = true;
    std::function<void()>                 handler_;
    virtual void                          rearm() = 0;

 public:
    explicit ievent(std::function<void()>&& handler, std::chrono::steady_clock::time_point when)
        : when_(when)
        , handler_(std::move(handler)) {}
    void trigger() {
        if (is_actual_) {
            handler_();
            rearm();
        }
    }
    auto get_when() const {
        return when_;
    }
    auto get_isActual() const {
        return is_actual_;
    }
    void cancel() {
        is_actual_ = false;
    }
    virtual ~ievent() = default;
};

using pevent = std::shared_ptr<ievent>;

pevent set_timeout(std::function<void()>&& handler, std::chrono::steady_clock::duration timeout);
// force_start==true - first call will be immediately
pevent set_interval(std::function<void()>&& handler, std::chrono::steady_clock::duration period);
pevent set_interval(std::function<void()>&& handler, std::chrono::steady_clock::duration period,
    std::chrono::steady_clock::duration start_delay);

void init();
void loop();

} // namespace event_loop
