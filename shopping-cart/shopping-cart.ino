#include <SoftwareSerial.h>
#include <AFMotor.h>
#include "SPI.h"
#include "MFRC522.h"
#include <DFRobotDFPlayerMini.h>
#include "VoiceRecognitionV3.h"

// Speaker
SoftwareSerial MP3Module(A14, A15); // 34, 35
DFRobotDFPlayerMini MP3Player;

// Microphone
VR myVR(10,11);    // 2:RX 3:TX,
uint8_t records[7];
uint8_t buf[64];

int START = 0;
int OBJ_1 = 1;
int OBJ_2 = 2;
int OBJ_3 = 3;
int CROSS = 4;
int START_POINT = START;
int DESTINATION = START;

int trig = 22;
int echo = 23;

int but_1 = 24; 
int but_2 = 25; 
int but_3 = 26; 

int piezo = 27;
int led_G = 28;
int led_R = 29;
int led_Y = 30;
int led_B = 31;

int IRL = 32; 
int IRM = 33; 
int IRR = 34; 

int count_1 = 0;
int cart_state = 0; 

// Motor
AF_DCMotor MOTOR1(1); // M1
AF_DCMotor MOTOR2(2); // M2
AF_DCMotor MOTOR3(3); // M3
AF_DCMotor MOTOR4(4); // M4

void Stop_Release() {
  MOTOR1.run(RELEASE);
  MOTOR2.run(RELEASE);
  MOTOR3.run(RELEASE);
  MOTOR4.run(RELEASE);
  delay(20);
}
void Go_Forward() {
  MOTOR1.run(BACKWARD);
  MOTOR2.run(BACKWARD);
  MOTOR3.run(FORWARD);
  MOTOR4.run(BACKWARD);
  delay(20);
}
void Go_Backward() {
  MOTOR1.run(FORWARD);
  MOTOR2.run(FORWARD);
  MOTOR3.run(BACKWARD);
  MOTOR4.run(FORWARD);
  delay(20);
}
void Go_Left() {
  MOTOR1.run(BACKWARD); // MOTOR1.run(FORWARD); 
  MOTOR2.run(FORWARD); // MOTOR2.run(BACKWARD);
  MOTOR3.run(BACKWARD); // MOTOR3.run(BACKWARD);
  MOTOR4.run(BACKWARD); // MOTOR4.run(FORWARD);
  delay(20);
}
void Go_Right() {
  MOTOR1.run(FORWARD); // MOTOR1.run(BACKWARD);
  MOTOR2.run(BACKWARD); // MOTOR2.run(FORWARD); 
  MOTOR3.run(FORWARD); // MOTOR3.run(FORWARD); 
  MOTOR4.run(FORWARD); // MOTOR4.run(BACKWARD);
  delay(20);
}
void Line_Trace() {
  // 0 0 0 
  if (digitalRead(IRL)==LOW && digitalRead(IRM)==LOW && digitalRead(IRR)==LOW)  { // 0 0 0
    delay(20);
    Go_Right();
    Serial.println("0 0 0");
  }
  if (digitalRead(IRL)==HIGH && digitalRead(IRM)==LOW && digitalRead(IRR)==HIGH)  { // 1 0 1
    delay(20);
    Go_Right();
    Serial.println("1 0 1");
  }
  if (digitalRead(IRL)==LOW && digitalRead(IRM)==LOW && digitalRead(IRR)==HIGH)  { // 0 0 1
    delay(20);
    Go_Right();
    Serial.println("0 0 1");
  }
  if (digitalRead(IRL)==LOW && digitalRead(IRM)==HIGH && digitalRead(IRR)==HIGH)  { // 0 1 1
    delay(20);
    Go_Right();
    Serial.println("0 1 1");
    //delay(1000);
  }
  if (digitalRead(IRL)==HIGH && digitalRead(IRM)==LOW && digitalRead(IRR)==LOW)  { // 1 0 0
    delay(20);
    Go_Left();
    Serial.println("1 0 0");
  }
  if (digitalRead(IRL)==HIGH && digitalRead(IRM)==HIGH && digitalRead(IRR)==LOW)  { // 1 1 0
    delay(20);
    Go_Left();
    Serial.println("1 1 0");
  }
  if (digitalRead(IRL)==HIGH && digitalRead(IRM)==HIGH && digitalRead(IRR)==HIGH)  { // 1 1 1
    delay(20);
    Go_Forward();
    Serial.println("1 1 1");
  }
  if (digitalRead(IRL)==LOW && digitalRead(IRM)==HIGH && digitalRead(IRR)==LOW)  { // 0 1 0
    delay(20);
    Go_Forward();
    Serial.println("0 1 0");
  }

}
// RFID
// SDA 53 | SCK 52 | MOSI 51 | MISO 50 | RST 49
#define SDA_PIN 53
#define RST_PIN 49
MFRC522 mfrc522(SDA_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

int isKey() {
  if (mfrc522.PICC_IsNewCardPresent() & mfrc522.PICC_ReadCardSerial()) {

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
      {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
      }
    String rfid = "";
    for (byte i = 0; i < 4; i++) {
      rfid +=
      (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(mfrc522.uid.uidByte[i], HEX) +
      (i!=3 ? ":" : "");
    }
  
    Serial.print("Card key: ");
    Serial.println(rfid);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    if (rfid == "c0:8f:9e:25" & DESTINATION == START) {
      cart_state = 0;
      Stop_Release();
    }
    if (rfid == "e8:ea:cf:0d" & DESTINATION == OBJ_1) {
      cart_state = 0;
      Stop_Release();
    }
    if (rfid == "e8:a0:6f:0d" & DESTINATION == OBJ_2) {
      cart_state = 0;
      Stop_Release();
    }
    if (rfid == "fb:df:c7:22" & DESTINATION == OBJ_3) {
      cart_state = 0;
      Stop_Release();
    }
    delay(100);
  }
}


void setup() {
  Serial.begin(9600);
  myVR.begin(9600);
  MP3Module.begin(9600);
  if (!MP3Player.begin(MP3Module)) { 
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  MP3Player.volume(30);  
  MP3Player.play(2);
  delay(1000);

  pinMode(trig, OUTPUT); 
  pinMode(echo, INPUT); 

  pinMode(led_R, OUTPUT); // LED RED
  pinMode(led_Y, OUTPUT); // LED YELLOW
  pinMode(led_B, OUTPUT); // LED BLUE
  pinMode(led_G, OUTPUT);

  pinMode(but_3, INPUT); 
  pinMode(but_2, INPUT);
  pinMode(but_1, INPUT);

  pinMode(IRR, INPUT);
  pinMode(IRM, INPUT);
  pinMode(IRL, INPUT);

  MOTOR1.setSpeed(255);
  MOTOR2.setSpeed(255);
  MOTOR3.setSpeed(255);
  MOTOR4.setSpeed(255);

  MOTOR1.run(RELEASE);
  MOTOR2.run(RELEASE);
  MOTOR3.run(RELEASE);
  MOTOR4.run(RELEASE);

  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  
  isKey();
  
  int val_x = analogRead(A8);
  int val_y = analogRead(A9);
  //Serial.print(val_x);
  //Serial.print(", ");
  //Serial.println(val_y);
  
  if (val_x < 10) {
    Serial.println("To Start Line");
    DESTINATION = START;
    MP3Player.play(1);
  }
  if (val_y < 10) {
    Serial.println("To Object 1 (Coke)");
    MP3Player.play(3);
    DESTINATION = OBJ_1;
  }
  if (val_x > 1000){
    Serial.println("To Object 2 (Snack)");
    MP3Player.play(4);
    DESTINATION = OBJ_2;
  }
  if (val_y > 1000){
    Serial.println("To Object 3 (Pineapple)");
    MP3Player.play(5);
    DESTINATION = OBJ_3;
  }
  
  
  if (count_1 >= 10000) {count_1 = 0;}
  
  if (digitalRead(but_1)==HIGH) {
    count_1 += 1;
    delay(20);
    if (count_1 % 2 == 1) {
      digitalWrite(led_G, HIGH);
      Serial.println("Mic operates");
      int ret;
      ret = myVR.recognize(buf, 50);
      if(ret>0)
      {
        
        Serial.print(buf[1]);
        Serial.println("");
        if (buf[1] == 6) {
          Serial.println("To Start Line");
          MP3Player.play(1);
        }
        if ((buf[1] == 0) | (buf[1] == 1)) {
          Serial.println("To Object 1 (Coke)");
          MP3Player.play(3);
        }
        if ((buf[1] == 2) | (buf[1] == 3)) {
          Serial.println("To Object 2 (Snack)");
          MP3Player.play(4);
        }
        if ((buf[1] == 4) | (buf[1] == 5)) {
          Serial.println("To Object 3 (Pineapple)");
          MP3Player.play(5);
        }
      }

    }
    if (count_1 % 2 == 0) {
  
      digitalWrite(led_G, LOW);
      Serial.println("Mic X");
    }
  }
  
  if (digitalRead(but_2)==HIGH) {
    //delay(20);
    cart_state = 2;
  }
  if (digitalRead(but_3)==HIGH) { 
    //delay(20);
    cart_state = 0;                                             
  }

  float len, distance;
  digitalWrite(trig, LOW); // Initialization
  delay(2);
  digitalWrite(trig, HIGH );
  delay(10);
  digitalWrite(trig, LOW);

  len = pulseIn(echo, HIGH);
  distance = ((float)(340*len)/10000)/2;
  //Serial.println(distance);

  if (cart_state == 2 & distance<=10) { 
    cart_state = 1; 
    Stop_Release();
    tone(piezo, 494);
    MP3Player.play(7);
  }
  if (distance>10 & cart_state == 1) { 
    cart_state = 2; 
    noTone(piezo);
    //Line_Trace();
    Go_Forward();
  }
  
  if (cart_state == 0){ 
    digitalWrite(led_R, HIGH);
    digitalWrite(led_Y, LOW);
    digitalWrite(led_B, LOW);
    Stop_Release();  
  }
  if (cart_state == 1) {
    digitalWrite(led_R, LOW);
    digitalWrite(led_Y, HIGH);
    digitalWrite(led_B, LOW); 
    Stop_Release();  
  }
  if (cart_state == 2) { 
    digitalWrite(led_R, LOW);
    digitalWrite(led_Y, LOW);
    digitalWrite(led_B, HIGH);   
    //Line_Trace();  
    Go_Forward();
  }
  
  delay(100);

}
