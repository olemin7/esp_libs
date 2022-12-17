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
                                  uint64_t milliseconds) {
  const auto id = get_id();
  _list.emplace_back(
      event_t{id, millis() + milliseconds, milliseconds, handler});
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
  const auto ms = millis();
  auto it = _list.begin();
  while (it != _list.end()) {
    if (ms >= it->when) {  // lunch one by loop
      auto handler = it->handler;
      handler();
      if (it->delay) {  // set_interval
        it->when = ms + it->delay;
      } else {
        _list.erase(it);
      }
      return;
    } else {
      it++;
    }
  }
}

void cevent_loop::remove(const int16_t id) {
  auto it = _list.begin();
  while (it != _list.end()) {
    if (it->id == id) {
      _list.erase(it);
      return;
    } else {
      it++;
    }
  }

  std::remove_if(_list.begin(), _list.end(),
                 [id](const auto& el) -> bool { return el.id == id; });
}

cevent_loop::~cevent_loop() {
  // TODO Auto-generated destructor stub
}
