/*
 * cledstatus.h
 *
 *  Created on: Dec 17, 2022
 *      Author: oleksandr
 */

#pragma once
#include <chrono>
#include <memory>
#include <stdint.h>

namespace led_status {
using namespace std::chrono_literals;

class igenerator {
public:
  virtual bool next() = 0;
  virtual std::chrono::steady_clock::duration period() = 0;
  virtual ~igenerator() = default;
};

class cled_status {
public:
  // clang-format off
    enum class value_t {
        On ,
        Off ,
        Warning ,
        Work
    };

  // clang-format on
  void setup();
  void set(value_t en);
  void set(std::unique_ptr<igenerator> gen);
  cled_status();
  ~cled_status();

private:
  class implementation;
  std::unique_ptr<implementation> impl_;
};

} // namespace led_status
