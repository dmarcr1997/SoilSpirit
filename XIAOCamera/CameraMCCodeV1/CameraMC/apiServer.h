#ifndef API_SERVER_H
#define API_SERVER_H

#include <WebServer.h>
#include <Arduino.h>
// #include <ESPmDNS.h>
#include "claudeAPI.h"
#include "esp_camera.h"

extern String lastCommand;

void captureAndAnalyzeImage();
void setupApiServer();
void handleAPIServer();

#endif //API_SERVER_H