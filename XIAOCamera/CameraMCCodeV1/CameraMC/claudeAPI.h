#ifndef CLAUDE_API_H
#define CLAUDE_API_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "secrets.h"

void analyzeImageWithClaude(const String& base64Image, const String& lastCommand);
bool sendClaudeRequest(const String& payload, String& result);

#endif //CLAUDE_API_H