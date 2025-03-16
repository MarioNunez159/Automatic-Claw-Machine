//Reference: "CameraWebServer" example code for AI Thinker ESP32-CAM board
//board and libraries provided by board package "esp32" (version 3.1.1) by Espressif Systems for Arduino IDE

#include "esp_camera.h"         
#include <WiFi.h>

#define CAMERA_MODEL_AI_THINKER           //model for ESP32_CAM

#include "camera_pins.h"                  //holds predefined GPIO values for EPS32_CAM (under CAMERA_MODEL_AI_THINKER within the file)

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "moto g 5G (2022)";    //hotspot network name
const char *password = "mnune072";

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;                 //GPIO for camera pins using predefined GPIO values in "camera_pins.h"
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
  config.frame_size = FRAMESIZE_240X240;    //outputs 240x240 frame for image captures
  config.pixel_format = PIXFORMAT_RGB565;   //PIXFORMAT_JPEG is an alternative format, but PIXFORMAT_RGB565 gets color better
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;                 //image quality - camera crashed too much when set to lower quality value (lower = better quality)
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config); //initialize camera
  if (err != ESP_OK) {                      //camera fails to start
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  WiFi.begin(ssid, password);         //start Wi-Fi connection
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {   //wait until camera connects to wifi
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();                      //start server upon connection

  Serial.print("Camera Ready! Use 'http://"); //provides website link to camera view
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  delay(10000);   //Camera outputs via the server, so nothing needs to be done here.
}
