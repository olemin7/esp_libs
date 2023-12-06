/*
 * CMQTT.cpp
 *
 *  Created on: 29 ????. 2017 ?.
 *      Author: User
 */
#ifdef CLOCK_CMQTT_CPP
#include "CMQTT.h"
#include "logs.h"
#include "wifiHandle.h"
#include <sstream>
using namespace std;

CMQTT::CMQTT()
    : client(espClient)
    , m_ClientID("ESP8266Client" __DATE__ __TIME__) {}

void CMQTT::setup(const char* domain, uint16_t port, const char* aClientID) {
    DBG_OUT << "MQTT Server:" << domain << ", port:" << std::dec << port << ", ClientID:" << aClientID << std::endl;
    m_ClientID = aClientID;
    client.setServer(domain, port);
    preState = client.connected();
}
bool CMQTT::connect() {
    if (client.connected()) {
        DBG_OUT << "already connected" << std::endl;
        connection_changed(true);
        return true;
    }
    DBG_OUT << "Attempting MQTT connection..." << std::endl;
    // Attempt to connect
    if (client.connect(m_ClientID)) {
        DBG_OUT << "connected" << std::endl;
        // ... and resubscribe
        if (!m_cb_topic.empty()) {
            client.subscribe(m_cb_topic.c_str());
        }
        connection_changed(true);
        return true;
    } else {
        DBG_OUT << "failed, rc=" << client.state() << std::endl;
        connection_changed(false);
        return false;
    }
}
void CMQTT::loop() {
    const bool state = isConnected();
    if (preState != state) {
        connection_changed(state);
    }
    if (state) {
        client.loop();
    }
}
bool CMQTT::publish(const string& topic, const string& message) {
    DBG_OUT << "topic=" << topic << ", message=" << message << std::endl;
    return client.publish(topic.c_str(), message.c_str());
}
#endif
