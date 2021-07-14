void saveConfig() {
  DynamicJsonDocument json(512);
  json["mqtt_server"] = mqtt_server;
  json["username"] = username;
  json["password"] = password;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    return;
  }

  serializeJson(json, configFile);
  configFile.close();
}

void loadConfig() {
  if (SPIFFS.begin()) {

    if (SPIFFS.exists("/config.json")) {
      File configFile = SPIFFS.open("/config.json", "r");

      if (configFile) {
        const size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument json(512);

        if (DeserializationError::Ok == deserializeJson(json, buf.get())) {
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(username, json["username"]);
          strcpy(password, json["password"]);
        }
      }
    }
  }
}
