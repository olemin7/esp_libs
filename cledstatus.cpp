/*
 * cledstatus.cpp
 *
 *  Created on: Dec 17, 2022
 *      Author: oleksandr
 */

#include "eventloop.h"
#include <Arduino.h>
#include <cledstatus.h>
#include <logs.h>
namespace led_status {

using namespace std::chrono_literals;
class cled_status::implementation {
private:
  event_loop::pevent handler_;
  std::unique_ptr<igenerator> gen_;
  void next() {
    const bool is_on = gen_->next();
    digitalWrite(LED_BUILTIN, !is_on); // 0 -turn on
    const auto period = gen_->period();
    if (period != 0s) {
      handler_ = event_loop::set_timeout([this]() { next(); }, period);
    }
  }

public:
  void set(std::unique_ptr<igenerator> gen) {
    if (handler_) {
      handler_->cancel();
    }
    gen_ = std::move(gen);
    next();
  }
};

class gen_off : public igenerator {
public:
  virtual bool next() override { return false; }
  virtual std::chrono::steady_clock::duration period() override { return 0s; }
};

class gen_on : public igenerator {
public:
  virtual bool next() override { return true; }
  virtual std::chrono::steady_clock::duration period() override { return 0s; }
};

class gen_warning : public igenerator {
private:
  bool on = true;

public:
  virtual bool next() override {
    on = !on;
    return on;
  }
  virtual std::chrono::steady_clock::duration period() override {
    return 100ms;
  }
};

class gen_work : public igenerator {
private:
  bool on = true;

public:
  virtual bool next() override {
    on = !on;
    return on;
  }
  virtual std::chrono::steady_clock::duration period() override {
    return (on) ? 5ms : 5s;
  }
};

cled_status::cled_status() : impl_(std::make_unique<implementation>()) {}
cled_status::~cled_status() = default;

void cled_status::setup() { pinMode(LED_BUILTIN, OUTPUT); }

void cled_status::set(value_t en) {
  switch (en) {
  case value_t::On:
    impl_->set(std::make_unique<gen_on>());
    break;
  case value_t::Off:
    impl_->set(std::make_unique<gen_off>());
    break;
  case value_t::Warning:
    impl_->set(std::make_unique<gen_warning>());
    break;
  case value_t::Work:
    impl_->set(std::make_unique<gen_work>());
    break;
  default:
    DBG_OUT << "incorrect enum" << static_cast<int>(en) << std::endl;
  }
}

void cled_status::set(std::unique_ptr<igenerator> gen) {
  impl_->set(std::move(gen));
}
} // namespace led_status
