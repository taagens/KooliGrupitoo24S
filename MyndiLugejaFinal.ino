#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>

#define RST_PIN 5  
#define SS_PIN 4   
const char* ssid = "A";      // Mnu wifi nimi : jagatud moblaga - wifi nimeks A
const char* password = "Ainar123";  // wifi parool
const char* server = "vepa.haridustee.ee";  // veebilehe URL - kuhu info läheb
Adafruit_NeoPixel pixels(10, D3, NEO_GRB + NEO_KHZ800); // teeme NeoPixel objekti

MFRC522 mfrc522(SS_PIN, RST_PIN);  
WiFiClientSecure client;  

int points = 0;  
unsigned long lastCheckTime = 0;  // Viimase kontrolli aeg
const unsigned long checkInterval = 300000;  // 5 minutit millisekundites sekundiga jooksis kokku

unsigned long lastReadTime = 0;   // Viimase RFID lugemise aeg
const unsigned long readInterval = 1000;  // Viivitus 1 sekund (1000 ms)

void setup() {
  Serial.begin(9600);  // Seriaalühenduse kiirus
  SPI.begin();         // SPI ühenduse initsialiseerimine
  mfrc522.PCD_Init();  
  Serial.println("Ootan RFID kaarti...");
  pixels.begin();
  pixels.clear();
  tuled(50,  100);
  // WiFi ühendus
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Igno
  client.setInsecure();
}

void loop() {
  // Kontrolli, kas 5 minni möödas
  if (millis() - lastCheckTime >= checkInterval) {
    checkIfDatabaseCleared();  
    lastCheckTime = millis();  
  }

  
  if (millis() - lastReadTime >= readInterval) {
    
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      
      // Loome stringi RFID-tagi ID-st
      String content = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      content.toUpperCase();  // Muudame kõik tähed suurteks

      Serial.print("RFID tagi ID: ");
      Serial.println(content);

      // Kontrolli ainult esimesi 10 sümbolit RFID-tagi ID-st - viimased muutuvad
      String checkID = content.substring(0, 10);
      Serial.println(checkID);  

      // RFID-id - punktid kaartidel
      if (checkID == "46BE9B68F6") {
        points += 1;
        Serial.println("1 punkt lisatud");
      }
      else if (checkID == "41E4AB68F6") {
        points += 2;
        Serial.println("2 punkti lisatud");
      }
      else if (checkID == "4B12DB58F6") {
        points += 1;
        Serial.println("1 punkti lisatud");
      }
      else if (checkID == "42018B18F6") {
        points += 1;
        Serial.println("1 punkti lisatud");
      }
      else if (checkID == "432FB58F61") {
        points += 1;
        Serial.println("1 punkti lisatud");
      }   
      else if (checkID == "463FEB68F6") {
        points += 2;
        Serial.println("2 punkti lisatud");
      }
      else if (checkID == "42BBDB68F6") {
        points += 2;
        Serial.println("2 punkti lisatud");
      }
      else if (checkID == "48EB6B78F6") {
        points += 2;
        Serial.println("2 punkti lisatud");
      } 
      else if (checkID == "44196B58F6") {
        points += 3;
        Serial.println("3 punkti lisatud");
      }
      else if (checkID == "45DBBB28F6") {
        points += 3;
        Serial.println("3 punkti lisatud");
      }
      else if (checkID == "42D85B58F6") {
        points += 3;
        Serial.println("3 punkti lisatud");
      }
      else if (checkID == "42092B78F6") {
        points += 3;
        Serial.println("3 punkti lisatud");
      }
      else if (checkID == "4C61B78F61") {
        points += 4;
        Serial.println("4 punkti lisatud");
      }
      else if (checkID == "4B5ACB68F6") {
        points += 4;
        Serial.println("4 punkti lisatud");
      }
      else if (checkID == "4A151B68F6") {
        points += 4;
        Serial.println("4 punkti lisatud");
      }
      else if (checkID == "44EC0B38F6") {
        points += 4;
        Serial.println("4 punkti lisatud");
      }      
      else if (checkID == "46BE9B68F6") {
        points += 5;
        Serial.println("5 punkti lisatud");
      }
            else if (checkID == "4C247B78F6") {
        points += 5;
        Serial.println("5 punkti lisatud");
      }
            else if (checkID == "4B72AB58F6") {
        points += 5;
        Serial.println("5 punkti lisatud");
      }
            else if (checkID == "4CE6AB48F6") {
        points += 5;
        Serial.println("5 punkti lisatud");
      }      
      else if (checkID == "4277BB48F6") {
        points += 8;
        Serial.println("8 punkti lisatud");
      }
      else if (checkID == "4CDCFB38F6") {
        points += 16;
        Serial.println("16 punkti lisatud");
      }
      else {
        Serial.println("Tundmatu kaart!");
      }

      Serial.print("Kogutud punktid: ");
      Serial.println(points);
      tuled(points, 100);

      // Saada punktid veebilehele
      sendPointsToServer(points);

      // Lõpeta RFID kaartide töötlemine
      mfrc522.PICC_HaltA();

      // Uuenda viimase RFID-kaardi lugemise aega
      lastReadTime = millis();
    }
    //Serial.println("Ei näe mitte midagi ...");
  }
}

// Funktsioon punktide saatmiseks veebilehele
void sendPointsToServer(int points) {
  if (client.connect(server, 443)) {  // HTTPS kasutab porti 443
    String postData = "points=" + String(points);
    
    client.println("POST /points.php HTTP/1.1");
    client.println("Host: vepa.haridustee.ee");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);

    Serial.println("Punktid saadetud veebilehele.");
    client.stop();  // Sulge ühendus 
  } else {
    Serial.println("Ei saanud veebilehega ühendust.");
  }
}

// Funktsioon, mis kontrollib, kas andmebaas on tühjendatud
void checkIfDatabaseCleared() {
  if (client.connect(server, 443)) {
    client.println("GET /check_database.php HTTP/1.1");
    client.println("Host: vepa.haridustee.ee");
    client.println("Connection: close");
    client.println();

    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "CLEARED") {
          points = 0;  // Nullime punktide loenduri, kui andmebaas on tühjendatud
          Serial.println("Andmebaas tühjendatud, punktid nullitud.");
        }
      }
    }
    client.stop();
  } else {
    Serial.println("Ei saanud ühendust, et kontrollida andmebaasi olekut.");
  }
}

//void tuled (int punktid ,int MAXpunktid){
 //int LED_arv=map(punktid, 0,  MAXpunktid, 0, 9);
  //for (int i=0; i<LED_arv; i++){
    //pixels.setPixelColor(i, pixels.Color(0, 150,0));
  //}
  void tuled(int punktid, int MAXpunktid) {
int LED_arv = punktid / 10;
int max_led = 10;

for (int i = 0; i < LED_arv  && i < max_led; i++) {
int red = map(i, 0, max_led - 1, 255, 0);
int green = map(i, 0, max_led - 1, 0, 255);
pixels.setPixelColor(i, pixels.Color(red, green, 0));

for(int i = LED_arv; i < max_led; i++) {
  pixels.setPixelColor(i, pixels.Color(0, 0, 0));
}
pixels.show();
}
  }
  

