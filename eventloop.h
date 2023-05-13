/*
 * ceventloop.h
 *
 *  Created on: Oct 19, 2022
 *      Author: oleksandr
 */

#pragma once
#include <stdint.h>

#include <functional>
#include <set>
#include <vector>

class cevent_loop {
 private:
  struct event_t{
    int16_t id;
    uint64_t when;
    uint64_t delay = 0;
    std ::function<void()> handler;
  } ;
  std ::vector<event_t> _list;
  std ::set<int16_t> remove_ids_;
  int16_t get_id();

 public:
  cevent_loop();
  int16_t set_timeout(std ::function<void()> &&handler, uint64_t milliseconds);
//force_start==true - first call will be immediately
  int16_t set_interval(std ::function<void()> &&handler, uint64_t milliseconds,
                       bool force_start = false);
  void loop();
  void remove(const int16_t id);
  virtual ~cevent_loop();
};

