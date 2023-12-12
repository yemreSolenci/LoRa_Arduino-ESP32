#include <LoRa.h>
#include <SPI.h>

// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define BAND 433E6

//packet counter

int counter = 0;
String packet = "";

boolean waterRelay = false;

void setup() {
  //initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("LoRa Sender");
  startLoRA();
}

//Initialize LoRa module
void startLoRA() {

  while (!LoRa.begin(BAND) && counter < 5) {
    Serial.print(".");
    counter++;
    delay(1000);
  }
  if (counter == 5) {
    Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
  
  Serial.println("LoRa Initialization OK!");
  counter = 1;
}

void loop()
{

  // print RSSI of packet
  Serial.print(" | RSSI: ");
  Serial.println(LoRa.packetRssi());

  Serial.print("Sending packet: ");
  Serial.println(counter);

  LoRa.beginPacket();   //Send LoRa packet to receiver
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
