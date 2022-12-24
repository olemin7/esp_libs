/*
 * ceventloop.cpp
 *
 *  Created on: Oct 19, 2022
 *      Author: oleksandr
 */

#include "eventloop.h"

#include <Arduino.h>
#include <algorithm>

cevent_loop::cevent_loop() {
  // TODO Auto-generated constructor stub
}
int16_t cevent_loop::set_timeout(std ::function<void()> &&handler,
                                 uint64_t milliseconds) {
  const auto id = get_id();
  _list.emplace_back(event_t{id, millis() + milliseconds, 0, handler});
  return id;
}

int16_t cevent_loop::set_interval(std ::function<void()> &&handler,
                                  uint64_t milliseconds, bool force_start) {
  const auto id = get_id();
  _list.emplace_back(event_t{id, force_start ? 0 : (millis() + milliseconds),
                             milliseconds, handler});
  return id;
}

int16_t cevent_loop::get_id() {
  int16_t id = 1;
  for (const auto &el : _list) {
    if (id != el.id) {
      break;
    }
    id++;
  }
  return id;
}

void cevent_loop::loop() {
  if (!remove_ids_.empty()) {
    _list.erase(std::remove_if(_list.begin(), _list.end(),
                               [this](const auto &el) -> bool {
                                 return remove_ids_.count(el.id) != 0;
                               }),
                _list.end());
    remove_ids_.clear();
  }
  const auto ms = millis();
  for (auto &event : _list) {
    if (ms >= event.when) {  // lunch one by loop
      event.handler();
      if (event.delay) {  // set_interval
        event.when = ms + event.delay;
      } else {
        remove_ids_.emplace(event.id);
      }
    }
  }
}

void cevent_loop::remove(const int16_t id) {
  remove_ids_.emplace(id);
}

cevent_loop::~cevent_loop() {
  // TODO Auto-generated destructor stub
}
