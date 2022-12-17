/*
 * cledstatus.h
 *
 *  Created on: Dec 17, 2022
 *      Author: oleksandr
 */

#pragma once
#include <stdint.h>

class cled_status {
 private:
  uint8_t mask_ = 0;
  uint8_t pattern_;

 public:
  // clang-format off
  static const uint8_t On = 0xff;
  static const uint8_t Off = 0;
  static const uint8_t Warning = 0b01010101;
  static const uint8_t Work = 0b00000001;
  // clang-format on
  static const uint16_t period_ms = 100;
  void setup();
  void set(uint8_t pattern);
  void loop();
  cled_status() = default;
};

