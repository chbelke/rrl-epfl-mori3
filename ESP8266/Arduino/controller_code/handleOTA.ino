// OTA Functionality
void handleOTA(){
  ArduinoOTA.setPort(8266);
  ArduinoOTA.onStart([]()
  {
    char* type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
    // using SPIFFS.end()
    char message[30];
    sprintf(message, "Start updating: %s", type);
    client.publish(publishName, message);
    verbose_println(message);
    client.loop();
  });

  ArduinoOTA.onEnd([]()
  {
    client.publish(publishName, "Finished Updating");
    verbose_println("\nEnd");
    client.loop();
    delay(500);
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("Progress: %i/100", (progress / (total / 100)));
    if (progress % 10 < 1)
    {
      char message[30];
      sprintf(message, "Progress: %i/100", (progress / (total / 100)));
      client.publish(publishName, message);
      client.loop();
    }

  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      client.publish(publishName, "ERR: Auth Failed");
      verbose_println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      client.publish(publishName, "ERR: Begin Failed");
      verbose_println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      client.publish(publishName, "ERR: Connect Failed");
      verbose_println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      client.publish(publishName, "ERR: Finished Updating");
      verbose_println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      client.publish(publishName, "ERR: Receive Failed");
      verbose_println("End Failed");
    }
    client.loop();
  });
}

void enableOTA()
{
  verbose_println("Stopping Loop");
  client.publish(publishName, "ERR: Pausing functionality until updated");
  client.loop();
  ArduinoOTA.begin();
  verbose_println("Ready");
  verbose_print("IP address: ");
  verbose_println(stringIP);
  delay(1000);
}
