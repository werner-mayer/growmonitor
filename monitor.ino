#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <Arduino.h>
 
#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define higometro1 A0
#define DS18B20_OneWire 2
 
OneWire oneWire(DS18B20_OneWire); 
DallasTemperature sensortemp(&oneWire);

FirebaseData firebaseData1;
FirebaseData firebaseData2;

String path = "/growroom";

FirebaseData firebaseData;

FirebaseJson json;
 
void setup()
{
  sensortemp.begin();
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  firebaseData1.setBSSLBufferSize(1024, 1024);

  firebaseData1.setResponseSize(1024);

  firebaseData2.setBSSLBufferSize(1024, 1024);

  firebaseData2.setResponseSize(1024);


  if (!Firebase.beginStream(firebaseData1, path))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{    
    printTemperature();
    delay(1000);
    readHigometro1();
    delay(1000);
}

void readHigometro1() 
{
  String statusPlanta1 = "";
  int leituraPlanta1 = analogRead(higometro1);
  if (leituraPlanta1 > 0 && leituraPlanta1 < 400)
  {
    statusPlanta1 = "Solo umido, leitura: " + String(leituraPlanta1);
  }
 
  if (leituraPlanta1 > 400 && leituraPlanta1 < 800)
  {
    statusPlanta1 = "Umidade moderada, leitura: " + String(leituraPlanta1);
  }
 
  if (leituraPlanta1 > 800 && leituraPlanta1 <= 1024)
  {
    statusPlanta1 = "Solo seco, leitura: " + String(leituraPlanta1);
  }

  Firebase.setString(firebaseData2, path + "/umidade/planta1", statusPlanta1);
}

void printTemperature() { 
  sensortemp.requestTemperatures(); 
  float grausC = sensortemp.getTempCByIndex(0); 
  Firebase.setFloat(firebaseData1, path + "/temperatura", grausC);
}
