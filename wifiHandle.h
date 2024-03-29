/*
 * wifiHandle.h
 *
 *  30 dec-2020
 *      Author: ominenko
 */
#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <string>
#include <vector>
#include <ostream>
#include <functional>
#include "CSignal.h"

ostream& operator<<(ostream &os, const IPAddress &ip);
std::string to_str(const wl_status_t status);

typedef enum {
  er_ok = 0,
  er_no_parameters,
  er_fileNotFound,
  er_openFile,
  er_createFile,
  er_incorrectMode,
  er_errorResult,
  er_BuffOverflow,
  err_MarlinRead,
  er_FileIO,
  er_timeout,
  er_last
} te_ret;

void webRetResult(ESP8266WebServer &server, te_ret res);

class CWifiStateSignal: public SignalLoop<wl_status_t> {
    bool getValue(wl_status_t &val) override;

};

void wifiHandle_loop();
void wifiHandle_send_content_json(ESP8266WebServer &server, std::function<te_ret(std::ostream &out)> content);
void wifiHandle_sendlist(ESP8266WebServer &server);
void wifiHandle_connect(const std::string device_name, ESP8266WebServer &server,
                        bool pers);

void setup_wifi(const String &ssid, const String &pwd, const String &host_name,
                const WiFiMode_t &wifi_mode, bool pers);

void wifiList(std::ostream &out);
void wifi_status(std::ostream &out);
