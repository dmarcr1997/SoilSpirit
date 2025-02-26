#include "esp_camera.h"
#include <WiFi.h>
#include <Base64.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15 
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
// #define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
//#define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"
#include "secrets.h"

// ===========================
//LLM CREDS
const String claudeApiKey = CLAUDE_API_KEY; //in secrets.h file along with SSID and PASSWORD
String Prompt = "Given this image, identify possible obstacles and provide your analysis in the following JSON format only: { 'obstacles': [ { 'position': 'LEFT|MIDDLE|RIGHT', 'distance': 'CLOSE|MEDIUM|FAR', 'description': 'brief description' } ], 'command': 'FORWARD|FULL_STOP|TURN_LEFT|TURN_RIGHT', 'reasoning': 'brief explanation for the command'} Do not include any text outside of this JSON structure. Base your command on the horizontal position, distance, and tracked obstacles. Keep on path and ignore background and grass/plants.";

// Function prototypes
String encodeImageToBase64(const uint8_t* imageData, size_t imageSize);
void captureAndAnalyzeImage();
void analyzeImage(const String& base64Image);
bool sendPostRequest(const String& payload, String& result);
void processSerialCommands();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println("Camera Ready! Use 'AT-CAPTURE' command to capture and analyze an image");
}

void loop() {
  processSerialCommands();
  delay(100);
}

void processSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "AT-CAPTURE") {
      Serial.println("Received AT-CAPTURE command. Processing...");
      captureAndAnalyzeImage();
    }
  }
}

String encodeImageToBase64(const uint8_t* imageData, size_t imageSize) {
  return base64::encode(imageData, imageSize);
}

void captureAndAnalyzeImage() {
  Serial.println("Capturing image...");

  // Capture the image frame buffer
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  Serial.println("Image captured");
  String base64Image = encodeImageToBase64(fb->buf, fb->len); //Convert image to base64

  // Return the frame buffer after processing the image
  esp_camera_fb_return(fb); 

  if (base64Image.isEmpty()) {
    Serial.println("Failed to encode the image!");
    return;
  }
  analyzeImageWithClaude(base64Image);
}

void analyzeImageWithClaude(const String& base64Image) {
  Serial.println("Sending image for analysis to Claude LLM...");
  String imageMedia = "data:image/jpeg;base64," + base64Image;
  
  // Calculate the payload size approximation with added buffer for json
  int payloadSize = base64Image.length() + 2000;
  DynamicJsonDocument doc(payloadSize > 16384 ? 16384 : payloadSize);
  
  // Prepare the payload for the Claude API
  doc["model"] = "claude-3-opus-20240229";
  doc["max_tokens"] = 1000;
  doc["temperature"] = 0.5;

  // Create the messages array with a user message
  JsonArray messages = doc.createNestedArray("messages");
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";
  
  // Create the content array for the user message
  JsonArray content = userMessage.createNestedArray("content");
  
  // Add Prompt
  JsonObject textPart = content.createNestedObject();
  textPart["type"] = "text";
  textPart["text"] = Prompt;
  
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
    Serial.println("ERROR: Payload exceeds limit. Reduce image size or quality.");
    return;
  }
  
  Serial.print("Payload size: ");
  Serial.println(jsonPayload.length());
  
  // Send the request to Claude API
  if (sendClaudeRequest(jsonPayload, result)) {
    Serial.println("[Claude] Response received");
    
    // Parse the response from Claude
    DynamicJsonDocument responseDoc(8192);
    DeserializationError error = deserializeJson(responseDoc, result);
    
    if (!error) {
      // Extract the response content
      String responseContent = responseDoc["content"][0]["text"].as<String>();
      Serial.println("[Claude] Analysis result:");
      Serial.println(responseContent);
    } else {
      Serial.print("[Claude] JSON parsing error: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("[Claude] Request error: ");
    Serial.println(result);
  }
}

bool sendClaudeRequest(const String& payload, String& result) {
  HTTPClient http;
  http.begin("https://api.anthropic.com/v1/messages");
  
  http.addHeader("Content-Type", "application/json");
  http.addHeader("anthropic-version", "2023-06-01");
  http.addHeader("x-api-key", claudeApiKey);
  http.setTimeout(60000); // 60 second timeout
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    result = http.getString();
    Serial.println("HTTP Response Code: " + String(httpResponseCode));
    
    if (result.length() > 200) {
      Serial.println("Response Body (truncated): " + result.substring(0, 200) + "...");
      //TODO: EXTRACT PARTS NECESSARY AND SAVE EXTRA INFO TO SERVER SOMEWHERE
    } else {
      Serial.println("Response Body: " + result);
    }
    
    http.end();
    return true;
  } else {
    result = "HTTP request failed, response code: " + String(httpResponseCode);
    Serial.println("Error Code: " + String(httpResponseCode));
    Serial.println("Error Message: " + http.errorToString(httpResponseCode));
    http.end();
    return false;
  }
}