#include <Console.h>
#include "panel.h"

#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);

// Fill the dots one after the other with a color
void solidColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
 strip.show();

}

void onKeyPress(unsigned char key)
{
    Serial.println("KeyDown");
  Serial.print(key);
  Serial.println("");
      writeKey(key,HIGH);
  solidColor(strip.Color(255, 0, 0)); // Red
 
}

void onKeyUp(unsigned char key)
{
  Serial.println("KeyUp");
  Serial.print(key);
  Serial.println("");
   writeKey(key,LOW);
        solidColor(strip.Color(255, 0, 255)); // WHITE

}




const int resetPin =  12;
long last = 0;


void setup() {

  Bridge.begin();
  Console.begin();


  // put your setup code here, to run once:
  Serial1.begin(57600);
  Serial.begin(9600);
  pinMode(resetPin, OUTPUT);
  pinMode(1,OUTPUT);
  pinMode(0,INPUT);
  pinMode(13,OUTPUT);  
  digitalWrite(13,HIGH);
  last = millis();
  digitalWrite(12, HIGH);

  delay(100);
  strip.begin();
  fadeIn();
  fadeOut();
}


void loop() {
  
  pollUI();
}
void fadeIn(){
  writeLed(0,1);
  writeLed(1,1);
  writeLed(2,1);
  writeLed(3,1);
  writeLed(4,1);
  writeLed(4,1);
  writeLed(6,1);
  delay(1000);
  writeLed(0,2);
  writeLed(1,2);
  writeLed(2,2);
  writeLed(3,2);
  writeLed(4,2);
  writeLed(6,2);
}
void fadeOut(){
  
  delay(3000);//3 seconds
  writeLed(0,1);
  writeLed(1,1);
  writeLed(2,1);
  writeLed(3,1);
  writeLed(4,1);
  writeLed(4,1);
  writeLed(6,1);
  delay(300);
  writeLed(0,0);
  writeLed(1,0);
  writeLed(2,0);
  writeLed(3,0);
  writeLed(4,0);
  writeLed(4,0);
  writeLed(6,0);
}
