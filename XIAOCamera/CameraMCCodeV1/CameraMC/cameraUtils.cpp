#include "cameraUtils.h"

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

  return analyzeImageWithClaude(base64Image);
}