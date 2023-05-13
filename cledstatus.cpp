/*
 * cledstatus.cpp
 *
 *  Created on: Dec 17, 2022
 *      Author: oleksandr
 */

#include <Arduino.h>
#include <cledstatus.h>
#include <logs.h>

void cled_status::setup() { pinMode(LED_BUILTIN, OUTPUT); }

void cled_status::loop() {
  if (!mask_) {
    mask_ = 1;
  }

  const auto is_On = digitalRead(LED_BUILTIN) == 0;
  const auto set_On = (mask_ & pattern_) != 0;
  if (is_On != set_On) {
    digitalWrite(LED_BUILTIN, !set_On);  // 0 -turn on
  }

  mask_ = mask_ << 1;
}

void cled_status::set(uint8_t pattern) {
  mask_ = 0;
  pattern_ = pattern;
}
