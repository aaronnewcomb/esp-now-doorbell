//sender ESP32

#include <esp_now.h>
#include <WiFi.h>
#include "DFRobotDFPlayerMini.h"

#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex
#define HOSTNAME "IoT-Beuller"
esp_now_peer_info_t receiver;
#define CHANNEL 0

HardwareSerial DFPlayerSerial(2); // Use UART channel 2
DFRobotDFPlayerMini myDFPlayer;

unsigned long timeout = millis();
const int led = 2;
int num_files = 1;
int vol = 22; //Set volume value. From 0 to 30

uint8_t remoteMac[] = {0x5E, 0xCF, 0x7F, 0x80, 0x22, 0x4F}; // Replace with the AP MAC address of the receiver/
uint8_t data = 1;
bool retry = true;
int count = 1;
int retry_times = 10;

void printMacAddress(uint8_t* macaddr) {
  Serial.print("{");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    Serial.print(macaddr[i], HEX);
    if (i < 5) Serial.print(',');
  }
  Serial.println("};");
}

// Init ESP Now with fallback
void InitESPNow() {
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

// send data
void sendData() {
  //data++;
  const uint8_t *peer_addr = receiver.peer_addr;
  Serial.print("Sending: "); Serial.println(data);
  esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
  Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}

// callback when data is sent from sender to receiver
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    retry = false;
  } else {
    count++;
    if (count > retry_times) {
      retry = false;
      count = 1;
    }
  }
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, 1); //Make sure LED starts out off
  //mySoftwareSerial.begin(9600);
  DFPlayerSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  // This is the mac address of the sender in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  memcpy(receiver.peer_addr, remoteMac, 6);
  int addStatus = esp_now_add_peer(&receiver);
  if (addStatus == 0) {
    // Pair success
    Serial.println("Pair success");
  } else {
    Serial.println("Pair failed");
  }
  while (!myDFPlayer.begin(DFPlayerSerial)) {  // Use UART 1 to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(1000);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(vol);
}

void loop() {
  while (retry) { // This will prabably work on the ESP32, but not the ESP8266
    sendData();
    delay(50);
  }
  // Play the file
  myDFPlayer.play(1);
  // Go to sleep
  Serial.println(millis());
  Serial.println("Shutting down.");
  Serial.println("Going to sleep.");
  esp_deep_sleep_start();
  Serial.println("Sleep failed.");
}
