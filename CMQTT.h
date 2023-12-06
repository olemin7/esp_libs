/*
 * CMQTT.h
 *
 *  Created on: 29 ????. 2017 ?.
 *      Author: User
 */

#ifndef CLOCK_CMQTT_H_
#define CLOCK_CMQTT_H_
#include <PubSubClient.h>
#include <WiFiClient.h>

#include <functional>
#include <memory>
#include <string>

class CMQTT {
 protected:
    WiFiClient   espClient;
    PubSubClient client;
    const char*  m_ClientID;
    std::string  m_cb_topic;
    using on_connect_cb_t = std::function<void(bool connected)>;
    std::unique_ptr<on_connect_cb_t> connection_change_cb_;
    bool                             preState;
    void                             connection_changed(bool connected) {
        if (connection_change_cb_) {
            (*connection_change_cb_)(connected);
        }
        preState = connected;
    }

 public:
    CMQTT();

    void setup(const char* domain, uint16_t port, const char* aClientID = nullptr);
    void on_connection_change(on_connect_cb_t&& cb) {
        connection_change_cb_ = std::make_unique<on_connect_cb_t>(std::move(cb));
    }
    bool connect();
    void callback(std::string topic, MQTT_CALLBACK_SIGNATURE) {
        client.setCallback(callback);
        m_cb_topic = std::move(topic);
    }
    bool isConnected() {
        return client.connected();
    }
    void  loop();
    bool  publish(const std::string& topic, const std::string& message);
    auto& get_client() {
        return client;
    }
};

#endif /* CLOCK_CMQTT_H_ */
