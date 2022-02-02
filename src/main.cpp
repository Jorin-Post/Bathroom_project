#include <Arduino.h>
#include <Wire.h> //Slave
#include <IRremote.h>

IRsend irsend;

int tmpset = 18, humset = 80, lghtset = 50, rad = 4, sch = 7, R1 = 11, R2 = 10, R3 = 12, bui = 55, bin = 55, bov, lght = 55, hour = 55, min = 55, tmp = 55, hum = 55, hu[] = {60,60,60,60,60}, h =0, pheat = 0, t= 0;
bool ven = HIGH, heat = HIGH, win = LOW, pven = HIGH, pven1 = LOW, pwin = LOW, Flash = LOW, Fade = LOW;

void receiveEvent(int howMany) {
  while (Wire.available()) {
    hour = Wire.read();
    min = Wire.read();
    tmp = Wire.read();
    h = Wire.read();
    bin = Wire.read();
    bov = Wire.read();
  }
  hum = 0;
  hu[t] = h;
  t++;
  if (t < 5)
    t = 0;
  for (int i =0; i < 5; i++){
    hum += hu[i];
  }
  hum = (int) hum / 5;
  Serial.println();
  Serial.println(hum);
}

void dataRqst(){  
  Wire.write(bui);
  Wire.write(pheat);
}

void f(){
  Serial.write(0xff);      Serial.write(0xff);      Serial.write(0xff);
}

void page1(){
   
  //if (((analogRead(A0)*(4600/1024)-500)/100) < 40)
  bui =((analogRead(A1)*(4600/1024)-500)/100);
  lght = analogRead(A3)/ 10;

  Serial.print("t0.txt=");
  Serial.print("\""); Serial.print(hour); Serial.print("\""); f();
  Serial.print("t1.txt=");
  Serial.print("\""); Serial.print(min); Serial.print("\""); f();
  Serial.print("t2.txt=");
  Serial.print("\""); Serial.print(tmp); Serial.print("\""); f();
  Serial.print("t3.txt=");
  Serial.print("\""); Serial.print(bui); Serial.print("\""); f();
  Serial.print("t4.txt=");
  Serial.print("\""); Serial.print(bin); Serial.print("\""); f();
  Serial.print("t5.txt=");
  Serial.print("\""); Serial.print(bov); Serial.print("\""); f();
  Serial.print("t6.txt=");
  Serial.print("\""); Serial.print(hum); Serial.print("\""); f();
  Serial.print("t7.txt=");
  Serial.print("\""); Serial.print(lght); Serial.print("\""); f();
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

void light(){
  if (lght < lghtset && rad == HIGH){
    irsend.sendNEC(0xF7C03F, 32);
  }
  if (sch == LOW){
    irsend.sendNEC(0xF740BF, 32);
  } 
}

void setup() {
  Serial.begin(9600);
  Wire.begin(0x8);               
  Wire.onReceive(receiveEvent);
  Wire.onRequest(dataRqst);
  pinMode(rad, INPUT);pinMode(sch, INPUT);pinMode(R1, OUTPUT);pinMode(R2, OUTPUT);pinMode(R3, OUTPUT);
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
  light();
  if (pheat == 100){
    heat = LOW;
  }
  /*else if (tmp < (tmpset-1) && pheat == LOW){
    heat = HIGH;
  }
  else if (tmp > (tmpset+1) && pheat == LOW) {
    heat = LOW;
  }*/
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
}