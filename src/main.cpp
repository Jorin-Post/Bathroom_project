#include <Arduino.h>
#include <Wire.h> //Slave
#include <IRremote.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 9 

IRsend irsend;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int tmpset = 18, humset = 80, lghtset = 50,   R1 = 11, R2 = 10, R3 = 12, lght = 55, hour = 55, min = 55, hum = 55, pheat = 0, bui = 0, tmp = 0, time = 0;
bool ven = HIGH, heat = HIGH, win = LOW, pven = HIGH, pven1 = LOW, pwin = LOW, Flash = LOW, Fade = LOW;

void receiveEvent(int howMany) {
  int t = 0, h = 0;
  while (Wire.available()) {
    hour = Wire.read();
    min = Wire.read();
    t = Wire.read();
    h = Wire.read();
  }
  if (t > 0 && t < 50)
    tmp = t;
  if (h > 0 && h < 100)
    hum = h;
}

void dataRqst(){ 
  Wire.write(bui);
  Wire.write(pheat);
}

void f(){
  Serial.write(0xff);      Serial.write(0xff);      Serial.write(0xff);
}

void page1(){
  
  sensors.requestTemperatures();
  bui = sensors.getTempCByIndex(0);
  lght = analogRead(A3)/ 10;

  Serial.print("t0.txt=");
  Serial.print("\""); Serial.print(hour); Serial.print("\""); f();
  Serial.print("t1.txt=");
  Serial.print("\""); Serial.print(min); Serial.print("\""); f();
  Serial.print("t2.txt=");
  Serial.print("\""); Serial.print(tmp); Serial.print("\""); f();
  Serial.print("t3.txt=");
  Serial.print("\""); Serial.print(bui); Serial.print("\""); f();
  Serial.print("t6.txt=");
  Serial.print("\""); Serial.print(hum); Serial.print("\""); f();
/*
  if (time < 10){
    time ++;
    Serial.print("dim=100"); f();
  }
  else {
    time = 0;
    Serial.print("dim=0"); f();
  }*/
}

void page3(){
  Serial.print("t0.txt=");
  Serial.print("\""); Serial.print(tmpset); Serial.print("\""); f();
  Serial.print("t1.txt=");
  Serial.print("\""); Serial.print(humset); Serial.print("\""); f();
  Serial.print("t2.txt=");
  Serial.print("\""); Serial.print(lghtset); Serial.print("\""); f();
  if (ven == HIGH){
    Serial.print("b0.pic=15");  f();
  }
  if (ven == LOW){
    Serial.print("b0.pic=16");  f();
  }
  if (heat == HIGH){
    Serial.print("b2.pic=17");  f();
  }
  if (heat == LOW){
    Serial.print("b2.pic=18");  f();
  }
  if (win == HIGH){
    Serial.print("b3.pic=20");  f();
  }
  if (win == LOW){
    Serial.print("b3.pic=19");  f();
  }
}

void setup() {
  Serial.begin(9600);
  sensors.begin();
  Wire.begin(0x8);              
  Wire.onReceive(receiveEvent);
  Wire.onRequest(dataRqst);
  pinMode(R1, OUTPUT);pinMode(R2, OUTPUT);pinMode(R3, OUTPUT);
  digitalWrite(R1, HIGH); digitalWrite(R2, HIGH); digitalWrite(R3, LOW);
}

void loop() {
  while (Serial.available() > 0){
    int txt = Serial.read();
    switch (txt){
      case 1:
      page1();
      break;
      case 3:
      page3();
      break;
      case 10:
        irsend.sendNEC(0xF7C03F, 32); // on
        break;
      case 11:
        irsend.sendNEC(0xF740BF, 32); // off
        break;
      case 12:
        irsend.sendNEC(0xF700FF, 32); // bright
        break;
      case 13:
        irsend.sendNEC(0xF7807F, 32); // dim
        break;
      case 14:
        irsend.sendNEC(0xF720DF, 32); // white+
        break;
      case 15:
        irsend.sendNEC(0xF7A05F, 32); // white-
        break;
      case 16:
        irsend.sendNEC(0xF7609F, 32); // jellow+
        break;
      case 17:
        irsend.sendNEC(0xF7E01F, 32); // jellow-
        break;
      case 18:
      if (Flash == LOW){
        irsend.sendNEC(0xF7D02F, 32); // flash
        Flash = HIGH;
      }
      else if (Flash == HIGH){
        irsend.sendNEC(0xF7708F, 32); // flash
        Flash = LOW;
      }
        break;
      case 19:
      if (Fade == LOW){
        irsend.sendNEC(0xF7F00F, 32); // fade
        Fade = HIGH;
      }
      else if (Fade == HIGH){
        irsend.sendNEC(0xF7708F, 32); // fade
        Fade = LOW;
      }
        break;
      case 20:
        irsend.sendNEC(0xF7C837, 32); // quick
        break;
      case 21:
        irsend.sendNEC(0xF7E817, 32); // slow
        break;
      case 30:
      pven1 = HIGH;
      if (pven1 == HIGH)
        pven1 = LOW;
      if (ven == LOW) {
        pven = HIGH;
      }
      else {
        pven = LOW;
      }
        break;
      case 31:
      if (heat == LOW) {
        pheat = 0;
      }
      else {
        pheat = 100;
      }
        break;  
      case 32:
      if (pwin == LOW) {
        win = HIGH;
        pwin = HIGH;
      }
      else {
        win = LOW;
      }
        break;   
      case 33:
        tmpset ++;
        break;   
      case 34:
        tmpset --;
        break;    
      case 35:
        humset += 5;
        break;
      case 36:
        humset -= 5;
        break;
      case 37:
        lghtset += 50;
        break;
      case 38:
        lghtset -= 50;
        break;
     default:
     break;    
    }
  }
  if (pheat == 100){
    heat = LOW;
  }
  else {
    heat = HIGH;
  }
  digitalWrite(R1, heat);
  
  if (pven == LOW){
    ven = LOW;
  }
  else if (hum > humset && pven1 == LOW){
    ven = LOW;
  }
  else if (hum < (humset-10) && pven1 == LOW) {
    ven = HIGH;
  }
  else {
    ven = HIGH;
  }
  digitalWrite(R2, ven);
  delay(1000);
}