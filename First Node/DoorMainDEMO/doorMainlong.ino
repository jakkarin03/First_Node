String Name0 = "นางสาว สมหมาย";
String Name1 = "นาย สมศักดิ์";
String Name2 = "นาย สมจิตร";
String Name3 = "นางสาว สมใจ";
String Name4 = "นาย สมดั่งใจ เธอหรือยัง";
String Name5 = "นาย ภาระ เพื่อนพ้อง";
String Name6 = "นาย สมคิด เองจ้า";  
String Name7 = "นางสาว สมศักศรี";
String Name8 = "นางสาว สมคิด ดี";
String Name9 = "นางสาว สมคิด ไม่ดี";

int num = 0;
int Num0,Num1,Num2,Num3,Num4,Num5,Num6,Num7,Num8,Num9;

int pos = 0;
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <WiFiClientSecureAxTLS.h>

#include <Servo.h>
#define led D0

#define SS_PIN D8
#define RST_PIN D4
#define sw D1

Servo myservo;

MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;
int ARRAYindexUIDcard;
int EEPROMstartAddr;
long adminID = 1122539531;
bool beginCard = 0;
bool addCard = 1;
bool skipCard = 0;
int LockSwitch;
unsigned long CardUIDeEPROMread[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//char auth[] = "GPOW2IP_PXxk6SlVQ3m7aWJxj29bz54q"; //face

char auth[] = "UCb_2_rS4BVhF-2ITQWsBXJ5ofk9m20s"; // bothong

#define WIFI_SSID "ASUS"
#define WIFI_PASSWORD "somkid555"

//#define WIFI_SSID "Admin-Wifi-2.4GHz"
//#define WIFI_PASSWORD "seam2448860"

#define LINE_TOKEN "JjhT4eTmfqUCfoFV3p2dOWKUHyj3yEwhI3jHDXHc2fq"

WidgetLCD lcd(V0);

void setup() {
  myservo.attach(D3);
  myservo.write(0);

  pinMode (led,OUTPUT);
  
  Serial.begin(115200);
  pinMode(sw, INPUT_PULLUP);

  SPI.begin();
  mfrc522.PCD_Init();

  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);
  lcd.clear(); //Use it to clear the LCD Widget
  EEPROM.begin(512);
  DisplayWAiT_CARD();
  EEPROMreadUIDcard();

}

void loop() {


  

  if (digitalRead(sw) == LOW) {
    //myservo.write(10); //unlock
    lcd.print(0, 0, "BUTTON PRESSED ");
    lcd.print(0, 1, "   DOOR OPEN   ");
    digitalWrite(led,HIGH);
                              for ( pos ; pos < 90 ; pos++) {
                                  myservo.write(pos);
                                  delay (15);
                                }
    delay(3000);
    for (int z=0;z<10;z++) {
    digitalWrite(led,LOW);
    delay(100);
    digitalWrite(led,HIGH);
    delay(100);
    }
    //myservo.write(120); //lock
                                for ( pos ; pos > 0 ; pos--) {
                                    myservo.write(pos);
                                    delay (15);
                                  }
    digitalWrite(led,LOW);
    DisplayWAiT_CARD();
    
  }
  
  if (beginCard == 0) {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {  //Look for new cards.
      Blynk.run();
      return;
    }

    if ( ! mfrc522.PICC_ReadCardSerial()) {  //Select one of the cards.
      Blynk.run();
      return;
    }
  }

  //Read "UID".
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }

  if (beginCard == 1 || LockSwitch > 0)EEPROMwriteUIDcard();  //uidDec == adminID

  if (LockSwitch == 0) {
    //CardUIDeEPROMread.
    for (ARRAYindexUIDcard = 0; ARRAYindexUIDcard <= 9; ARRAYindexUIDcard++) {
      if (CardUIDeEPROMread[ARRAYindexUIDcard] > 0) {
        if (CardUIDeEPROMread[ARRAYindexUIDcard] == uidDec) {

          Serial.print("TURE");
          Serial.println(uidDec);
          
          lcd.print(0, 0, "CARD ACCESS OPEN");
          lcd.print(3, 1, uidDec);
          digitalWrite(led,HIGH);
                              for ( pos ; pos < 90 ; pos++) {
                                  myservo.write(pos);
                                  delay (15);
                                }
    delay(3000);
    for (int z=0;z<10;z++) {
    digitalWrite(led,LOW);
    delay(100);
    digitalWrite(led,HIGH);
    delay(100);
    }
    
                                for ( pos ; pos > 0 ; pos--) {
                                    myservo.write(pos);
                                    delay (15);
                                  }
    digitalWrite(led,LOW);
          CommandsCARD();
          break;
        }
      }
    }

    if (ARRAYindexUIDcard == 10) {

          Serial.print("NO");
          Serial.println(uidDec);
      
      lcd.print(0, 0, " Card not Found ");
      lcd.print(0, 1, "                ");
      lcd.print(0, 1, "ID : ");
      lcd.print(5, 1, uidDec);
      for (int i = 0; i <= 2; i++)delay(100); 
      delay(2000);
    }

    ARRAYindexUIDcard = 0;
    DisplayWAiT_CARD();
  }

  Blynk.run();
}

BLYNK_WRITE(V2) {
  int a = param.asInt();
  if (a == 1) {
    skipCard = 1;
    if (EEPROMstartAddr / 5 < 10) EEPROMwriteUIDcard();
  } else {
    skipCard = 0;
  }
}

BLYNK_WRITE(V3) {
  int a = param.asInt();
  if (a == 1) {
    lcd.print(0, 0, " APP UNLOCK OK ");
    lcd.print(0, 1, "   DOOR OPEN   ");
    digitalWrite(led,HIGH);
                              for ( pos ; pos < 90 ; pos++) {
                                  myservo.write(pos);
                                  delay (15);
                                }
    delay(3000);
    for (int z=0;z<10;z++) {
    digitalWrite(led,LOW);
    delay(100);
    digitalWrite(led,HIGH);
    delay(100);
    }
    
                                for ( pos ; pos > 0 ; pos--) {
                                    myservo.write(pos);
                                    delay (15);                                    
                                  }
    digitalWrite(led,LOW);
    DisplayWAiT_CARD();
  }
}

void EEPROMwriteUIDcard() {

  if (LockSwitch == 0) {
    lcd.print(0, 0, " START REC CARD ");
    lcd.print(0, 1, "PLEASE TAG CARDS");
    delay(500);
  }

  if (LockSwitch > 0) {
    if (skipCard == 1) {  //uidDec == adminID
      lcd.print(0, 0, "    SKIP   CARD");
      lcd.print(0, 1, "                ");
      lcd.print(0, 1, "   label : ");
      lcd.print(11, 1, EEPROMstartAddr / 5);
      EEPROMstartAddr += 5;
      skipCard = 0;
      num++;
    } else {
      Serial.println("writeCard");
      EEPROM.write(EEPROMstartAddr, uidDec & 0xFF);
      EEPROM.write(EEPROMstartAddr + 1, (uidDec & 0xFF00) >> 8);
      EEPROM.write(EEPROMstartAddr + 2, (uidDec & 0xFF0000) >> 16);
      EEPROM.write(EEPROMstartAddr + 3, (uidDec & 0xFF000000) >> 24);
      EEPROM.commit();
      num++;
      delay(10);
      lcd.print(0, 1, "                ");
      lcd.print(0, 0, "REC OK! IN CARD");
      lcd.print(0, 1, "MEMORY : ");
      lcd.print(9, 1, EEPROMstartAddr / 5);
      EEPROMstartAddr += 5;
      delay(500);
    }
  }

  LockSwitch++;

  if (EEPROMstartAddr / 5 == 10) {
    lcd.clear();
    lcd.print(0, 0, "FINISH");
    delay(2000);
    EEPROMstartAddr = 0;
    uidDec = 0;
    ARRAYindexUIDcard = 0;
    num = 0;
    EEPROMreadUIDcard();
  }
}

void EEPROMreadUIDcard() {
  for (int i = 0; i <= 9; i++) {
    byte val = EEPROM.read(EEPROMstartAddr + 3);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr + 2);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr + 1);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;

    ARRAYindexUIDcard++;
    EEPROMstartAddr += 5;
  }

  ARRAYindexUIDcard = 0;
  EEPROMstartAddr = 0;
  uidDec = 0;
  LockSwitch = 0;
  DisplayWAiT_CARD();
}

void DisplayWAiT_CARD() {
  lcd.print(0, 0, "   ATTACH THE   ");
  lcd.print(0, 1, "      CARD      ");
}

void Line_Notify(String message) {
axTLS::WiFiClientSecure client; //กรณีข้ึน Error ให้ลบ axTLS:: ขา้งหนา้ทิ้ง
if (!client.connect("notify-api.line.me", 443)) {
Serial.println("connection failed");
return;
}
String req = "";
req += "POST /api/notify HTTP/1.1\r\n";
req += "Host: notify-api.line.me\r\n";
req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
req += "Cache-Control: no-cache\r\n";
req += "User-Agent: ESP8266\r\n";
req += "Connection: close\r\n";
req += "Content-Type: application/x-www-form-urlencoded\r\n";
req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
req += "\r\n";
req += "message=" + message;
// Serial.println(req);
client.print(req);
delay(20);
// Serial.println("-------------");
while(client.connected()) {
String line = client.readStringUntil('\n');
if (line == "\r") {
break;
}
//Serial.println(line);
}
// Serial.println("-------------");
}

void CommandsCARD()
{
  
Serial.println(uidDec);

  if (uidDec == Num0)
  {
    Line_Notify(Name0);
  }

  else if (uidDec == Num1)
  {
    Line_Notify(Name1);
  }

  else if (uidDec == Num2)
  {
    Line_Notify(Name2);
  }

  else if (uidDec == Num3)
  {
    Line_Notify(Name3);
  }

  else if (uidDec == Num4)
  {
    Line_Notify(Name4);
  }

  else if (uidDec == Num5)
  {
    Line_Notify(Name5);
  }

  else if (uidDec == Num6)
  {
    Line_Notify(Name6);
  }

  else if (uidDec == Num7)
  {
    Line_Notify(Name7);
  }

  else if (uidDec == Num8)
  {
    Line_Notify(Name8);
  }

  else if (uidDec == Num9)
  {
    Line_Notify(Name9);
  }
  delay(1);
}

void SaveNum() {
  // put your main code here, to run repeatedly:
   if (num = 0) {
   Num0 = uidDec
   }
   if (num = 1) {
   Num1 = uidDec
   }
   if (num = 2) {
   Num2 = uidDec
   }
   if (num = 3) {
   Num3 = uidDec
   }
   if (num = 4) {
   Num4 = uidDec
   }
   if (num = 5) {
   Num5 = uidDec
   }
   if (num = 6) {
   Num0 = uidDec
   }
   if (num = 7) {
   Num1 = uidDec
   }
   if (num = 8) {
   Num2 = uidDec
   }
   if (num = 9) {
   Num3 = uidDec
   }
}
