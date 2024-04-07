#include <CTBot.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* wifiSSID = "INZAGHI";
const char* wifiPassword = "Mo7o#GP7";
const char* telegramToken = "6921680398:AAFSPV0t2KZKsxw5aiwKn6863LpQPAqcmG8";
const int telegramChatId = 1372872364;

const int waterFlowPin = D2;
volatile unsigned int pulseCount = 0;
float adress = 17;
float adress2 = 2;
float volume = 0;
const float litersPerPulse = 1.0 / 340.0;

CTBot myBot;
bool statusEn = true;
bool notificationSent = false;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
  volume = pulseCount * litersPerPulse;
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  pinMode(waterFlowPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(waterFlowPin), pulseCounter, RISING);

  // Koneksi ke WiFi
  WiFi.begin(wifiSSID, wifiPassword);
  myBot.setTelegramToken(telegramToken);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(EEPROM.get(adress, pulseCount));
  Serial.println(EEPROM.get(adress2, volume));
  Serial.println("Connected to WiFi");
}

void loop() {
  //TBMessage msg;
  char vol[10];
  dtostrf(volume, 4, 2, vol);

  const char* msg1 = "Volume hampir penuh, segera ganti filter. Volume saat ini (liters): ";
  const char* msg2 = "Volume penuh, ganti filter!. Volume saat ini (liters): ";

  int total1 = strlen(msg1) + strlen (vol) + 1;
  int total2 = strlen(msg2) + strlen(vol) + 1;
  
  char result1[total1];
  char result2[total2];

  strcpy(result1, msg1);
  strcat(result1, vol);

  strcpy(result2, msg2);
  strcat(result2, vol);


  if (WiFi.status() == WL_CONNECTED) {
   
    /*if(myBot.getNewMessage(msg)){
      if (msg.text.equals("cek")) {
        // If the message is "cek", send the current volume data to Telegram
        String replyMessage = "Current volume: " + String(buffer) + " liters";
        sendTelegramNotification(telegramChatId, replyMessage.c_str());
        delay(1000)
      }
    }*/
    if (volume > 5 && volume < 6 && !notificationSent) {
      Serial.println("Volume between 5 and 6 liters");
      sendTelegramNotification(telegramChatId, result1);
      notificationSent = true;
      delay(1000);
    }
    
      if (volume > 7 && statusEn == true) {
        Serial.println("Volume penuh");
        sendTelegramNotification(telegramChatId, result2);
        statusEn = false;  
        notificationSent = false;
        pulseCount = 0;
        volume = 0;
        delay(1000);
    }
  } else {
    Serial.println("Tidak ada koneksi Wi-Fi.");
  }

  statusEn = true;

  Serial.print("Flow Sensor Pulses: ");
  Serial.print(pulseCount);

  Serial.print(" Total Flow (liters): ");
  Serial.println(volume);

  Serial.println(EEPROM.put(adress, pulseCount));
  Serial.println(EEPROM.put(adress2, volume));

  EEPROM.commit();
  
  delay(1000);
}

void sendTelegramNotification(const int telegramChatId, const char* message) {
   String response = String(myBot.sendMessage(telegramChatId, message));
}
