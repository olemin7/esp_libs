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
    T m_lastVal;

   protected:
    void notify(const T &value) {
        for (auto &iterator : m_onSignal) {
            iterator(value);
        }
        if (m_firstTime || isChanged(value, m_lastVal)) {
          m_firstTime = false;
          m_lastVal = value;
          for (auto &iterator : m_onChange) {
            iterator(value);
          }
        }
    }
    virtual bool isChanged(const T &lhs, const T &rhs) {
        return lhs != rhs;
    }
public:
 T getLastValue() const { return m_lastVal; }
 void onSignal(std::function<void(const T &)> _handler) {
   m_onSignal.push_back(_handler);
 }
    void onChange(std::function<void(const T&)> _handler) {
        m_onChange.push_back(_handler);
    }
    virtual ~Signal() = default;
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
 virtual ~SignalLoop() = default;
};

//+send change on startup after debounce passed
template <class T>
class SignalDebounceLoop : public SignalLoop<T> {
 private:
  std::function<T()> get_raw_;
  const uint32_t event_timeout_;
  uint64_t timeout_;
  bool is_need_init_;
  T prevalue;

  virtual bool getValue(T &value) final {
    const auto ms = millis();
    const auto raw = get_raw_();
    if (!is_need_init_ && (raw == prevalue)) {
      timeout_ = 0;
    } else if (timeout_) {
      if (ms > timeout_) {
        is_need_init_ = false;
        value = prevalue = raw;  // debounce ok
        return true;
      }
    } else {
      timeout_ = ms + event_timeout_;
    }
    return false;
  }

 public:
  SignalDebounceLoop(uint32_t event_timeout, std::function<T()> &&get_raw)
      : get_raw_(std::move(get_raw)),
        event_timeout_(event_timeout),
        timeout_(0),
        is_need_init_(true){};
  virtual ~SignalDebounceLoop() = default;
};
