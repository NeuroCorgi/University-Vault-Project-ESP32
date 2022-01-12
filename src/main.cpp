#include <Arduino.h>
#include <EEPROM.h>

#include <cstring>

#include "bluetooth.h"
#include "network.h"

#define EEPROM_SIZE sizeof(Network::CONFIG)

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1);
    EEPROM.begin(EEPROM_SIZE);

    while (!Serial2.available());
    Serial.println("Available");

    String initBuff = Serial2.readString();
    Serial.println(initBuff);

    if (initBuff == "startup") {
        Serial.println("Startup");
        EEPROM.get(0, Network::CONFIG);
        for (int i = 0; i < 33; i++) {
            Serial.print(Network::CONFIG.SSID[i]);
        }
        Serial.println();
        Network::init();

    } else if (initBuff == "setup") {
        Serial.println("Setup");
        Bluetooth::init();

        Serial.println("Start waiting");
        while (!Bluetooth::dataWritten) {
            Serial.println(Bluetooth::dataWritten);
            delay(1000);
        }
        
        EEPROM.put(0, Network::CONFIG);
        EEPROM.commit();

        Network::init();
        Serial.println("Network");
    }
}

void loop() {
    Network::loop();
    if (Serial2.available()) {
        String buff = Serial2.readString();
        Serial.println(buff);
        if (buff == "attempt_1") {
            Network::sendLog(true);
        }
        else if (buff == "attempt_0") {
            Network::sendLog(false);
        }
        else if (buff == "req_2fa") {
            Serial.println("Req");
            Network::sendReq2FA();
        }
        else if (buff == "canc_2fa") {
            Serial.println("Canc");
            Network::sendCanc2FA();
        }
    }
}