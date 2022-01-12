#ifndef SERVER_H
#define SERVER_H

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h>

namespace Network {

    #define HOST "192.168.1.102"
    #define PORT 8000
    #define PATH "/ws/vault/"

    struct {
        char SSID[33];
        char password[64];
        char token[37];
    } CONFIG;

    WiFiMulti wifi;
    WebSocketsClient ws_client;

    void initWiFi() {
        wifi.addAP(CONFIG.SSID, CONFIG.password);

        Serial.print("Connecting");
        while (wifi.run() != WL_CONNECTED) {
            delay(100);
        }
        Serial.println();
    }
    
    void wsEvent(WStype_t event, uint8_t *payload, size_t len) {
        switch (event) {
        case WStype_DISCONNECTED:
            Serial.println("Disconnected");
            break;
        case WStype_CONNECTED:
            Serial.println("Connected");
            break;
        case WStype_TEXT:
            Serial.println("confirm_2fa");
            Serial2.print("confirm_2fa");
            break;
        
        default:
            break;
        }
    }

    void initWebsocket() {
        String url = PATH;
        url += CONFIG.token;
        url += "/";

        ws_client.begin(HOST, PORT, url.c_str());
        ws_client.onEvent(wsEvent);
        ws_client.setReconnectInterval(2500);
    }

    void init() {
        initWiFi();
        delay(2500);
        initWebsocket();
    }

    void sendReq2FA() {
        if (ws_client.isConnected()) {
            ws_client.sendTXT("{\"type\": \"2fa_req\"}");
        }
    }

    void sendCanc2FA() {
        if (ws_client.isConnected()) {
            ws_client.sendTXT("{\"type\": \"2fa_canc\"}");
        }
    }

    void sendLog(bool type) {
        if (ws_client.isConnected()) {
            char request[40];
            sprintf(request, "{\"type\": \"log\", \"success\": %s}", type ? "true" : "false");
            ws_client.sendTXT(request);
        }
    }

    void loop() {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.disconnect();
            WiFi.reconnect();
        }

        ws_client.loop();
    }
}

#endif /* SERVER_H */