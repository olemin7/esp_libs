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
int16_t cevent_loop::on_timeout(uint64_t milliseconds,
                                std ::function<void()> handler) {
  const auto id = get_id();
  _list.emplace_back(event_t{id, millis() + milliseconds, handler});
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
      _list.erase(it);
      handler();
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
