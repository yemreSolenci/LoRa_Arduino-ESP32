#include <LoRa.h>
#include <SPI.h>

// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define BAND 433E6

//packet counter
int counter = 0;
String LoRaData = "";

boolean waterRelay = false;


void setup() {
  pinMode(22, OUTPUT);
  digitalWrite(22, LOW);
  //initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("LoRa Receiver");
  startLoRA();
  digitalWrite(22, HIGH);
}

//Initialize LoRa module
void startLoRA() {
  LoRa.setPins(14, 12, 2);

  while (!LoRa.begin(BAND) && counter < 5) {
    Serial.print(".");
    counter++;
    delay(1000);
  }
  if (counter == 5) {
    Serial.println("LoRa init failed. Check your connections.");
    delay(100);
    while (1) {
      digitalWrite(22, LOW);
      delay(300);
      digitalWrite(22, HIGH);
      delay(500);
      digitalWrite(22, LOW);
      delay(300);
      digitalWrite(22, HIGH);
      delay(500);
      digitalWrite(22, LOW);
      delay(300);
      digitalWrite(22, HIGH);

      delay(10000);
    }
  }

  Serial.println("LoRa Initialization OK!");
  counter = 1;
  delay(2000);
}

void loop() {

  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    digitalWrite(22, LOW);
    Serial.print("Received packet '");

    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    
    // print RSSI of packet
    Serial.print(F("' | RSSI: "));
    Serial.println(LoRa.packetRssi());
    
    digitalWrite(22, HIGH);

    counter++;
  }

}
