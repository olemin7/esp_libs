/*
 * CObserver.h
 *
 *  Created on: 13 may  2021
 *      Author: ominenko
 */

#pragma once

#include <stdint.h>
#include <vector>
#include <functional>
using namespace std;
/***
 *
 */

template<class T>
class Signal {
private:
    std::vector<std::function<void(const T&)> > m_onSignal;
    std::vector<std::function<void(const T&)> > m_onChange;
    bool m_firstTime = true; //for first mine init;
    T m_preVal;
    protected:
    void notify(const T &value) {
        for (auto &iterator : m_onSignal) {
            iterator(value);
        }
        if (m_firstTime || isChanged(value, m_preVal)) {
            m_firstTime = false;
            m_preVal = value;
            for (auto &iterator : m_onChange) {
                iterator(value);
            }
        }
    }
    virtual bool isChanged(const T &lhs, const T &rhs) {
        return lhs != rhs;
    }
public:
    T getPreValue() const {
        return m_preVal;
    }
    void onSignal(std::function<void(const T&)> _handler) {
        m_onSignal.push_back(_handler);
    }
    void onChange(std::function<void(const T&)> _handler) {
        m_onChange.push_back(_handler);
    }
    ~Signal() {
    }
};

template<class T>
class SignalLoop: public Signal<T> {
private:
    virtual bool getValue(T &value) =0;
    public:
    virtual void loop() {
        T value;
        if (getValue(value)) {
            this->notify(value);
        }
    }
public:
    ~SignalLoop() {
    }
};

