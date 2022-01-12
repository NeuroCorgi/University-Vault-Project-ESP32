#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "network.h"

namespace Bluetooth {

    BLEServer *pServer = NULL;
    BLEService *pReadService;
    BLECharacteristic *pReadCharacteristic;
    BLEService *pWriteService;
    BLECharacteristic *pWriteCharacteristic;
    bool deviceConnected = false;
    bool oldDeviceConnected = false;
    bool dataWritten = false;

    #define READ_SERVICE_UUID           "FFD0"
    #define WRITE_SERVICE_UUID          "FFD5"
    #define READ_CHARACTERISTIC_UUID    "FFD4"
    #define WRITE_CHARACHTERISTIC_UUID  "FFD9"

    class ServerCallbacks: public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {
            deviceConnected = true;
        };

        void onDisconnect(BLEServer* pServer) {
            deviceConnected = false;
        };
    };

    class WriteCallbacks: public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pCharacteristic)  {
            std::string val = pCharacteristic->getValue();

            if (val.length() > 0) {

                std::string SSID = val.substr(0, val.find(":"));
                val.erase(0, val.find(":") + 1);
                std::string password = val.substr(0, val.find(":"));
                val.erase(0, val.find(":") + 1);
                std::string token = val;
                
                for (int i=0; i < SSID.length(); i++) Serial.print(SSID[i]);
                Serial.println();
                for (int i=0; i < password.length(); i++) Serial.print(password[i]);
                Serial.println();
                for (int i=0; i < token.length(); i++) Serial.print(token[i]);
                Serial.println();

                std::strcpy(Network::CONFIG.SSID, SSID.c_str());
                std::strcpy(Network::CONFIG.password, password.c_str());
                std::strcpy(Network::CONFIG.token, token.c_str());

                dataWritten = true;
                Serial.println(dataWritten);
            }
        };
    };

    void start();

    void init() {
        BLEDevice::init("Fort.Vault-0001");

        pServer = BLEDevice::createServer();
        pServer->setCallbacks(new ServerCallbacks());

        // Create the BLE read Service
        pReadService = pServer->createService(READ_SERVICE_UUID);
        // Create the BLE read charachteristic
        pReadCharacteristic = pReadService->createCharacteristic(
            READ_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
        );

        // Create the BLE write Service
        pWriteService = pServer->createService(WRITE_SERVICE_UUID);
        // Create the BLE write charachteristic
        pWriteCharacteristic = pWriteService->createCharacteristic(
            WRITE_CHARACHTERISTIC_UUID,
            BLECharacteristic::PROPERTY_WRITE
        );
        pWriteCharacteristic->setCallbacks(new WriteCallbacks());

        start();
    }

    void start() {
        // Start the services
        pReadService->start();
        pWriteService->start();

        // Start advertising
        pServer->getAdvertising()->start();
    }

    void stop() {
        // Stop the services
        pReadService->stop();
        pWriteService->stop();

        // Stop advertising
        pServer->getAdvertising()->stop();
    }
}

#endif /* BLUETOOTH_H */