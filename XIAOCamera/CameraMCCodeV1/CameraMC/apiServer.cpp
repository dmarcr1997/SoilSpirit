#include "apiServer.h"

WebServer server(80); //Web server on port 80
String lastCommand = "";

String captureAndAnalyzeImage() {
  Serial.println("[Camera] Capturing image...");

  // Capture the image frame buffer
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[Camera] Camera capture failed");
    return "Capture Error";
  }

  Serial.println("[Camera] Image captured");
  String base64Image = encodeImageToBase64(fb->buf, fb->len); //Convert image to base64

  // Return the frame buffer after processing the image
  esp_camera_fb_return(fb); 

  if (base64Image.isEmpty()) {
    Serial.println("Failed to encode the image!");
    return "Encode Error";
  }
  return analyzeImageWithClaude(base64Image, lastCommand);
}

void handleRoot() {
    if (server.hasArg("lastCommand")) { //check for lastCommand query param
        // Get the query parameter value
        String commandParam = server.arg("lastCommand");
        lastCommand = commandParam;
        String command = captureAndAnalyzeImage();
        Serial.println("[Server] Sent Command");
        server.send(200, "text/plain", command);
    } else {
        server.send(200, "text/plain", "Incorrect Query Params");
    }
}


void setupApiServer() {
  // Define server routes
  server.on("/", HTTP_GET, handleRoot);

  // Start the server
  server.begin();
  Serial.println("[Server] HTTP server started");
}

// Handle API server in the main loop
void handleAPIServer() {
  server.handleClient();
}