/*
 * wifiHandle.cpp
 *
 *  30 dec-2020
 *      Author: ominenko
 */
#ifdef WIFIHANDLE_CPP
#include "wifiHandle.h"
#include "logs.h"
#include "misk.h"
#include <iostream>
#include <sstream>
using namespace std;

ostream& operator<<(ostream &os, const IPAddress &ip) {
    os << ip.toString().c_str();
    return os;
}

std::string to_str(const wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD:
      return "WL_NO_SHIELD";
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
    case WL_WRONG_PASSWORD:
      return "WL_WRONG_PASSWORD";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
  }
  std::string unknown;
  unknown = static_cast<unsigned>(status);
  return unknown + "unknown";
}

void webRetResult(ESP8266WebServer &server, te_ret res)
        {
    DBG_OUT << "Err:" << (unsigned) res << endl;
    switch (res) {
        case er_ok:
            server.send(200, "text/plain", "ok");
            break;
        case er_fileNotFound:
            server.send(404, "text/plain", "FileNotFound");
            break;
        case er_openFile:
            server.send(404, "text/plain", "er_openFile");
            break;
        case er_createFile:
            server.send(500, "text/plain", "er_createFile");
            break;
        case er_incorrectMode:
            server.send(400, "text/plain", "er_incorrectMode");
            break;
        case er_no_parameters:
            server.send(400, "text/plain", "er_no_parameters");
            break;
        case er_errorResult:
            server.send(401, "text/plain", "er_errorResult");
            break;
        case er_BuffOverflow:
            server.send(400, "text/plain", "er_BuffOverflow");
            break;
        case err_MarlinRead:
            server.send(400, "text/plain", "err_MarlinRead");
            break;
        case er_FileIO:
            server.send(400, "text/plain", "er_FileIO");
            break;
        case er_timeout:
            server.send(400, "text/plain", "er_timeout");
            break;
        default:
            server.send(400, "text/plain", "undefined");
            break;
    }
}

bool CWifiStateSignal::getValue(wl_status_t &val) {
    val = WiFi.status();
    return true;
}
void wifiHandle_send_content_json(ESP8266WebServer &server, std::function<te_ret(std::ostream &out)> content) {
    DBG_FUNK();
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.sendHeader("Content-Type", "application/json", true);
    server.sendHeader("Cache-Control", "no-cache");
    ostringstream line;
    content(line);
    server.sendContent(line.str().c_str());
    server.sendContent("");
}

te_ret wifiHandle_sendlist_content(std::ostream &out) {
    DBG_FUNK();

    out << "{\"AP_LIST\":[";
    const auto count = WiFi.scanNetworks(false, true);
    DBG_OUT << "networks:" << count << endl;

    for (int i = 0; i < count; i++) {
        if (i) {
            out << ",";
        }
        out << "{\"SSID\":\"" << WiFi.SSID(i).c_str() << "\",";
        out << "\"SIGNAL\":\"" << WiFi.RSSI(i) << "\",";
        out << "\"IS_PROTECTED\":\"" << ((ENC_TYPE_NONE == WiFi.encryptionType(i)) ? 0 : 1) << "\",";
        out << "\"IS_HIDDEN\":\"" << WiFi.isHidden(i) << "\"}";
    }
    out << "]}";
    return er_ok;
}

void wifiHandle_sendlist(ESP8266WebServer &server) {
    DBG_FUNK();
    wifiHandle_send_content_json(server, wifiHandle_sendlist_content);
}

static bool showed = false;
void wifiHandle_loop() {
    if (showed) {
        return;
    }
    if (WIFI_STA == WiFi.getMode()) {
        if (WiFi.status() == WL_CONNECTED) {
            showed = true;
            DBG_OUT << "IP address:" << WiFi.localIP() << ", RSSI:" << WiFi.RSSI() << endl;
        }
    }
}

void wifiHandle_connect(const std::string device_name, ESP8266WebServer &server,
                        bool pers) {
  DBG_FUNK();
  if (server.hasArg("mode") && (server.arg("mode") == "WIFI_AP")) {
    String pwd = "";
    if (server.hasArg("pwd")) {
      pwd = server.arg("pwd");
    }
    webRetResult(server, er_ok);

    WiFi.persistent(pers);
    delay(500);  // to sent reply

    WiFi.mode(WIFI_AP);
    WiFi.softAP(device_name.c_str(), pwd);
    DBG_OUT << "start AP=" << device_name << ", pwd=" << pwd
            << ",ip:" << WiFi.softAPIP() << ", pers=" << pers << endl;

  } else {
    String ssid;
    String pwd = "";
    // sta
    if (!server.hasArg("ssid")) {
      webRetResult(server, er_no_parameters);
      return;
    }
    ssid = server.arg("ssid");
    if (server.hasArg("pwd")) {
      pwd = server.arg("pwd");
    }

    WiFi.persistent(pers);
    delay(500);
    DBG_OUT << "connecting ssid=" << ssid << ", pwd=" << pwd
            << ", device:" << device_name << ", pers=" << pers << std::endl;

    WiFi.hostname(device_name.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pwd.c_str());
    const auto ms = millis() + 5000;
    bool err_exit = false;
    while ((millis() < ms) && (!err_exit)) {
      DBG_OUT << ".";
      switch (WiFi.status()) {
        case WL_CONNECTED:
          webRetResult(server, er_ok);
          return;
        case WL_CONNECT_FAILED:
        case WL_NO_SSID_AVAIL:
        case WL_WRONG_PASSWORD:
          err_exit = true;
          return;
        default:
          break;
      }
      delay(500);
    }
  }
}

void setup_wifi(const String &ssid, const String &pwd, const String &host_name,
        const WiFiMode_t &wifi_mode) {
    // Set hostname first
    DBG_OUT << "hostname:" << host_name << ",pwd:" << pwd << ",Mode:" << WiFi.getPhyMode() << endl;
    WiFi.hostname(host_name);
    // Reduce startup surge current
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.mode(wifi_mode);
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);
    if (WIFI_STA == wifi_mode) {
        WiFi.begin(ssid.c_str(), pwd.c_str());
    } else {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(host_name.c_str(), pwd.c_str());
    }
}

/*******************************************************************************
 *
 */
void wifiList(std::ostream &out) {
    out << "Wifi scaning...";
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    out << std::endl;
    for (int i = 0; i < n; ++i)
        out << "-" << WiFi.SSID(i) << "(" << WiFi.RSSI(i) << ")"
                << ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*")
                << std::endl;
    out << "Scan is done. Networks " << n << std::endl;
}

void wifi_status(std::ostream &out) {
    out << "WiFi: mode=" << WiFi.getMode();
    if (WIFI_STA == WiFi.getMode()) {
      out << "(STA), SSID=" << WiFi.SSID() << ", status=" << WiFi.status()
          << "(" << to_str(WiFi.status()) << ")";
      if (WL_CONNECTED == WiFi.status()) {
        out << ", ip=" << WiFi.localIP();
        }
    } else {
        out << "(AP), host= " << WiFi.hostname() << ", ip=" << WiFi.softAPIP();
    }
    out << endl;
}
#endif
