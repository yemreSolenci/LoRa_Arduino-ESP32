#include <LoRa.h>
#include <SPI.h>

// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define BAND 433E6

#define led 8
#define button 7

int ledState;

int counter = 0;  // packet counter

// Bylnk ile gelen veri için
String LoRaData = "";
String ledOn = "led on";
String ledOff = "led off";


void setup() {
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  //initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("LoRa Receiver");

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
    Serial.println("LoRa init failed. Check your connections.");
    delay(100);
    while (1) {
      delay(10000);
    }
  }

  Serial.println("LoRa Initialization OK!");
  counter = 0;
  delay(2000);
}


//  Drive LED
void updateLEDState() {
  if (ledState == true)
  {
    digitalWrite(led, HIGH);
    Serial.println("led on");
  } else {
    digitalWrite(led, LOW);
    Serial.println("led off");
  }
}

void loop()
{
  if (digitalRead(button) == LOW)  // Fiziksel butona basıldı
  {
    ledState = !ledState;

    // ledin durumunu değiştirilmesi
    updateLEDState();

    // Bylnk senkronizasyonu için durum değişikliğinin gönderilmesi
    LoRa.beginPacket();   // Send LoRa packet
    LoRa.print("led ");
    ledState ? LoRa.print("on") : LoRa.print("off");
    LoRa.endPacket();
    counter++;
    Serial.print("Sending packet: ");
    Serial.println(counter);

    delay(500);
  }

  //  Lora gelen paket varmı kontrol etme
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    while (LoRa.available()) {
      LoRaData += (char)LoRa.read();
    }

    //  Alınan paketi yaz
    Serial.print("Received packet '");
    Serial.print(LoRaData);

    // print RSSI of packet
    Serial.print(F("' | RSSI: "));
    Serial.println(LoRa.packetRssi());

    // Toggle led state
    if (LoRaData.equals(ledOn)) {
      ledState = 1;
      updateLEDState();
      //Blynk.virtualWrite(V0, rcvBtnState);
      //Serial.println("ver v0: "+ String(rcvBtnState));
    } else if (LoRaData.equals(ledOff)) {
      ledState = 0;
      updateLEDState();
    } else  Serial.println("Hatalı mesaaj");

    LoRaData = "";  //  Gelecek bir sonraki paket için Stringi temizle

    counter++;
  }
}
