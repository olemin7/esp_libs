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
    WiFiClient espClient;
    PubSubClient client;
    const char *m_ClientID;
    std::string m_cb_topic;
    using on_connect_cb_t = std::function<void(bool connected)>;

  public:
    CMQTT();

    void setup(const char *domain, uint16_t port, const char *aClientID = nullptr);
    void connect(on_connect_cb_t &&cb);
    void callback(std::string topic, MQTT_CALLBACK_SIGNATURE) {
        client.setCallback(callback);
        m_cb_topic = std::move(topic);
    }
    bool isConnected() {
        return client.connected();
    }
    void loop() {
        if (isConnected()) {
          client.loop();
        }
    }
    bool publish(const std::string &topic, const std::string &message);
    auto &get_client() { return client; }
};

#endif /* CLOCK_CMQTT_H_ */
