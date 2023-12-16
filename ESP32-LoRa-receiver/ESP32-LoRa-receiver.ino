/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL6WddJA5Ye"
#define BLYNK_TEMPLATE_NAME         "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "vp3SkhKg9DO26s-OZx8_wz6cimDEUran"

#include <LoRa.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32_SSL.h>


//  Kablosuz ağ bilgileri
const char* ssid     = "Xiaomi 11T";
const char* password = "xgt8x56w";

// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define BAND 433E6

#define btn 4


int counter = 0;  //packet counter
String LoRaData = "";   //  Gelen veriler için
String ledOn = "led on";
String ledOff = "led off";

void setup() {
  pinMode(22, OUTPUT);  // Durum göstergesi için build-in ledi kullandım
  digitalWrite(22, LOW);  // Initialize bölümününde led yanar
  //pinMode(btn, INPUT_PULLUP);
  
  //initialize Serial Monitor
  Serial.begin(9600);  
  Serial.println("LoRa Sender");
  Serial.println();
  Serial.print("[WiFi] Connecting to "); 
  Serial.print(ssid);
  
  //connectWiFi();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  
  Serial.println("   " + WiFi.localIP());
  
  startLoRA();
  digitalWrite(22, HIGH);
}

void connectWiFi() {
  WiFi.begin(ssid, password);

  int tryDelay = 500;
  int numberOfTries = 20;

  // Wait for the WiFi event
  while (true) {

    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection was lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan is completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.print("[WiFi] Failed to connect to WiFi!");
      // Use disconnect function to force stop trying to connect
      WiFi.disconnect();
      return;
    } else {
      numberOfTries--;
    }
  }
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
      // Initialize LoRa başarısız olduğunda uyarı için led yanıp söner
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
  counter = 0;
  delay(2000);
}

//  Drive LED
void sendPacket(String data){
  LoRa.beginPacket();   // Send LoRa packet
  LoRa.print(data);
  LoRa.endPacket();
  counter++;
  Serial.print("Sending packet: "+ data);
  Serial.println(counter);
}

//  Bylnk'ten veri geldiğinde çalışır
BLYNK_WRITE(V0) {   
  digitalWrite(22, LOW);
  // integer olarak değer al ve değere göre packet gönder
  param.asInt() == true ? sendPacket("led on") : sendPacket("led off");
  digitalWrite(22, HIGH);
}

void loop() {
  Blynk.run();

  //  Lora gelen paket varmı kontrol etme
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    digitalWrite(22, LOW);
    Serial.print("Received packet '");

    while (LoRa.available()) {
      LoRaData += (char)LoRa.read();
    }
    
    Serial.print(LoRaData);

    // print RSSI of packet
    Serial.print(F("' | RSSI: "));
    Serial.println(LoRa.packetRssi());


    // Send state to Bylnk for sync
    if (LoRaData.equals(ledOn))
    {
      Blynk.virtualWrite(V0, 1);
      Serial.println("sent Bylnk v0: 1");
    }   else if (LoRaData.equals(ledOff)) {
      Blynk.virtualWrite(V0, 0);
      Serial.println("send Bylnk v0: 0");
    } else  Serial.println("Hatalı mesaaj");

    LoRaData = "";  //  Gelecek bir sonraki paket için Stringi temizle
    counter++;

    digitalWrite(22, HIGH);
  }
}
