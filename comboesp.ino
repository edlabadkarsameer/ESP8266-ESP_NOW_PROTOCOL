//this is the source code for ESP8266 
//for connecting two or more ESP8266 in duplex manner
//we are using ESP NOW protocol (as MOD BUS protocol)
//uploded by sameer edlabadkar 


#include<ESP8266WiFi.h>
#include<espnow.h>

#define MY_ROLE         ESP_NOW_ROLE_COMBO              // set the role of this device: CONTROLLER, SLAVE, COMBO
#define RECEIVER_ROLE   ESP_NOW_ROLE_COMBO              // set the role of the receiver
#define WIFI_CHANNEL    1

#define MY_NAME         "SENSOR NODE"
uint8_t receiverAddress[] = {0xA4, 0xCF, 0x12, 0xC7, 0xEB, 0xFC};     // for esp no. 1 

//#define MY_NAME         "PC NODE"
//uint8_t receiverAddress[] = {0x48, 0x3F, 0xDA, 0xA3, 0x86, 0xF8};   // for esp no. 2

struct __attribute__((packed)) dataPacket {
  int sensor1;
  int sensor2;
  float sensor3;
};

void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if(transmissionStatus == 0) {
    Serial.println("Data sent successfully");
  } else {
    Serial.print("Error code: ");
    Serial.println(transmissionStatus);
  }
}

void dataReceived(uint8_t *senderMac, uint8_t *data, uint8_t dataLength) {
  char macStr[18];
  dataPacket packet;  

  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);

  Serial.println();
  Serial.print("Received data from: ");
  Serial.println(macStr);
  
  memcpy(&packet, data, sizeof(packet));
  
  Serial.print("sensor1: ");
  Serial.println(packet.sensor1);
  Serial.print("sensor2: ");
  Serial.println(packet.sensor2);
  Serial.print("sensor3: ");
  Serial.println(packet.sensor3);
}
 
void setup() {
  Serial.begin(115200);     // initialize serial port

  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print(MY_NAME);
  Serial.println("...initializing...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();        // we do not want to connect to a WiFi network
//sameer edlabadkar 
  if(esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_set_self_role(MY_ROLE);   
  esp_now_register_send_cb(transmissionComplete);         // this function will get called once all data is sent
  esp_now_register_recv_cb(dataReceived);               // this function will get called whenever we receive data
  esp_now_add_peer(receiverAddress, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);

  Serial.println("Initialized.");
}

void loop() {
  dataPacket packet;
  
  packet.sensor1 = 789;
  packet.sensor2 = 1011;
  packet.sensor3 = 6.28;

  esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet));

  delay(3000);
}
