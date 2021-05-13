/*
 * CLightSensor.h
 *
 *  Created on: 11 ???. 2017 ?.
 *      Author: User
 */

#pragma once
#include <Arduino.h>
#include <stdint.h>
#include <array>
#include "CSignal.h"
/*
 PhotoR     10K
 +3    o---/\/\/--.--/\/\/---o GND
 |
 Pin 0 o-----------
 */

class CADC_filter: public SignalLoop<int> {
    std::array<int, 10> m_filter;
    uint8_t m_count = 0; //average count
    static constexpr int m_refreshPeriod = 100; // ms
    unsigned long m_nextRead = 0;
    const int m_Pin;
    const int m_Tolerance;
    public:
    CADC_filter(int _Pin = A0, int tolerance = 5) :
            m_Pin(_Pin), m_Tolerance(tolerance) {
    }
    ;
    bool isChanged(const int &lhs, const int &rhs) override {
        return (lhs > (rhs + m_Tolerance)) || (lhs < (rhs - m_Tolerance));
    }
    void setup();
    bool getValue(int &val);
};

