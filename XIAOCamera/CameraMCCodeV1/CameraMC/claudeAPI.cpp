#include "claudeAPI.h"

String Prompt = "Given this image, identify possible obstacles and provide your analysis in the following JSON format only: { 'obstacles': [ { 'position': 'LEFT|MIDDLE|RIGHT', 'distance': 'CLOSE|MEDIUM|FAR', 'description': 'brief description' } ], 'command': 'FORWARD|FULL_STOP|TURN_LEFT|TURN_RIGHT', 'reasoning': 'brief explanation for the command'} Do not include any text outside of this JSON structure. Base your command on the horizontal position, distance, tracked obstacles, and passed in last command. Keep on path and ignore background and grass/plants.";
const String claudeAPIKey = CLAUDE_API_KEY;

String analyzeImageWithClaude(const String& base64Image, const String& lastCommand) {
  Serial.println("Sending image for analysis to Claude LLM...");
  String imageMedia = "data:image/jpeg;base64," + base64Image;
  
  // Calculate the payload size with added buffer for json
  int payloadSize = base64Image.length() + 2000;
  DynamicJsonDocument doc(payloadSize > 16384 ? 16384 : payloadSize);
  
  // Claude Params
  doc["model"] = "claude-3-opus-20240229";
  doc["max_tokens"] = 1000;
  doc["temperature"] = 0.5;

  // Message Array
  JsonArray messages = doc.createNestedArray("messages");
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";
  
  // Conten Array
  JsonArray content = userMessage.createNestedArray("content");
  
  // Add Prompt
  JsonObject textPart = content.createNestedObject();
  textPart["type"] = "text";
  textPart["text"] = Prompt + "########## LAST COMMAND: " + lastCommand;
  
  // Add Base64 Image
  JsonObject imagePart = content.createNestedObject();
  imagePart["type"] = "image";
  JsonObject source = imagePart.createNestedObject("source");
  source["type"] = "base64";
  source["media_type"] = "image/jpeg";
  source["data"] = base64Image;
  
  // Serialize to string
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  
  String result;
  
  // Check payload size
  if (jsonPayload.length() > 100000) {
    Serial.println("ERROR: Payload exceeds limit. Reduce image size");
    return "[Image] Error";
  }
  
  Serial.print("Payload size: ");
  Serial.println(jsonPayload.length());
  
  // Send the request to LLM API
  if (sendClaudeRequest(jsonPayload, result)) {
    Serial.println("[LLM] Response received");
    
    // Parse the response
    DynamicJsonDocument responseDoc(8192);
    DeserializationError error = deserializeJson(responseDoc, result);
    
    if (!error) {
      String responseContent = responseDoc["content"][0]["text"].as<String>();
      Serial.println("[LLM] Analysis result:");
      Serial.println(responseContent);
      
      // Extract command
      DynamicJsonDocument textDoc(4096);
      DeserializationError textError = deserializeJson(textDoc, responseContent);
      if (!textError && textDoc.containsKey("command")) {
        String commandResp = textDoc["command"].as<String>();
        Serial.println("Sending Command: ");
        Serial.println(commandResp);
        return commandResp;
      }
    } else {
      Serial.print("[LLM] Parsing error: ");
      Serial.println(error.c_str());
      return error.c_str();
    }
  } else {
    Serial.print("[LLM] API Request error: ");
    Serial.println(result);
    return result;
  }
}

bool sendClaudeRequest(const String& payload, String& result) {
  HTTPClient http;
  http.begin("https://api.anthropic.com/v1/messages");
  
  http.addHeader("Content-Type", "application/json");
  http.addHeader("anthropic-version", "2023-06-01");
  http.addHeader("x-api-key", claudeAPIKey);
  http.setTimeout(60000); // 60 second timeout
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    result = http.getString();
    Serial.println("[LLM] HTTP Response Code: " + String(httpResponseCode));
    
    if (result.length() > 200) {
      Serial.println("[LLM] Response Body (truncated): " + result.substring(0, 200) + "...");
    } else {
      Serial.println("[LLM] Response Body: " + result);
    }
    
    http.end();
    return true;
  } else {
    result = "HTTP request failed, response code: " + String(httpResponseCode);
    Serial.println("[LLM] Error Code: " + String(httpResponseCode));
    Serial.println("[LLM] Error Message: " + http.errorToString(httpResponseCode));
    http.end();
    return false;
  }
}
