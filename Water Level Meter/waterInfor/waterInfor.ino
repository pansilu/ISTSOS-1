#include "Ultrasonic.h"

#define TRIGGER 10
#define ECHO 11

// Create Ultrasonic Object reader
Ultrasonic ultrasonic(TRIGGER, ECHO);

void setup() {
  Serial.begin(1200);
  delay(100);
}

void loop() {
  if(Serial.available()){
    if(Serial.read()=='1'){                     // request has pusshed
      Serial.print(ultrasonic.distanceRead());  // read data and send
      Serial.print('A');                       // End Charactor
    }
  }
}

