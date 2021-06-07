//receiver  ESP-8266
/***************************************************
 Using DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/index.php?route=product/product&search=mp3&description=true&product_id=1121>
 
 ***************************************************/
extern "C" {
#include <espnow.h>
}
//#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <ESP8266WiFi.h>
//CHANGE THIS TO A UNIQUE HOSTNAME ON YOUR NETWORK
#define HOSTNAME "IoT-Doorbell"
#define CHANNEL 0

SoftwareSerial mySoftwareSerial(4, 5); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

unsigned long timeout = millis();
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
int button_pin = 6;
const int led = 2;
int num_files = 1;
int vol = 30; //Set volume value. From 0 to 30
bool play_file = false;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, 1); //Make sure LED starts out off
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP_STA);
  // This is the mac address of the receiver in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());

  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
  
  while (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
  Serial.println(F("DFPlayer Mini online."));
  num_files = myDFPlayer.readFileCounts(); // total number of files on flash drive
  myDFPlayer.volume(vol);  
  // Randomize the randomness
  randomSeed(random(1,1001));
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

void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  play_file = true;
}

void loop() {
  if (play_file) {
     myDFPlayer.play(random(1, num_files));  //Play a random mp3
     play_file = false;
  }
  if (millis() - timeout > 3000) {
    Serial.println("Waiting for data ...");
    timeout = millis();
  }
}
