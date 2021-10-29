#pragma once

#include <ArduinoJson.h>
#include <FS.h>

#define JSON_SIZE 512

namespace Config {
    char mqtt_server[80] = "example.tld";
    char mqtt_topic[80] = "esp8266-vindriktning-particle-sensor/%s";

    char username[24] = "";
    char password[72] = "";


    void save() {
        DynamicJsonDocument json(JSON_SIZE);
        json["mqtt_server"] = mqtt_server;
        json["mqtt_topic"] = mqtt_topic;
        json["username"] = username;
        json["password"] = password;
      

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
            return;
        }

        serializeJson(json, configFile);
        configFile.close();
    }

    void load() {
        if (SPIFFS.begin()) {

            if (SPIFFS.exists("/config.json")) {
                File configFile = SPIFFS.open("/config.json", "r");

                if (configFile) {
                    const size_t size = configFile.size();
                    std::unique_ptr<char[]> buf(new char[size]);

                    configFile.readBytes(buf.get(), size);
                    DynamicJsonDocument json(JSON_SIZE);

                    if (DeserializationError::Ok == deserializeJson(json, buf.get())) {
                        strcpy(mqtt_server, json["mqtt_server"]);
                        strcpy(mqtt_topic, json["mqtt_topic"]);
                        strcpy(username, json["username"]);
                        strcpy(password, json["password"]);
                    }
                }
            }
        }
    }
} // namespace Config
