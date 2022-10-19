/*
 * ceventloop.h
 *
 *  Created on: Oct 19, 2022
 *      Author: oleksandr
 */

#pragma once
#include <stdint.h>

#include <functional>
#include <vector>

class cevent_loop {
 private:
  struct event_t{
    int16_t id;
    uint64_t when;
    std ::function<void()> handler;
  } ;
  std ::vector<event_t> _list;
  int16_t get_id();

 public:
  cevent_loop();
  int16_t on_timeout(uint64_t milliseconds, std ::function<void()> handler);
  void loop();
  void remove(const int16_t id);
  virtual ~cevent_loop();
};

