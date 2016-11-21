#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <EEPROM.h>

// function prototypes
void connect();
void battery_level();
void switch_change();

/****************************** Pins ******************************************/

#define BUTTON          12
#define Blue            5  // LED Color
#define Green            4  // LED Color
#define Red           13  //2  // LED Color

void setup() {
  // set LED pins as outputs
  pinMode(Blue, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);

  //make sure LEDs are OFF
  digitalWrite(Red, HIGH);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, HIGH);
  
  // set button pin as an input
  pinMode(BUTTON, INPUT_PULLUP);

  Serial.begin(115200);
  EEPROM.begin(512);
   
  Serial.println('\n');
  Serial.println(F("Waking up; checking states now."));
}  



void loop(){
  digitalWrite(Red, LOW);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, HIGH);
  delay(100);
  digitalWrite(Red, LOW);
  digitalWrite(Blue, LOW);
  digitalWrite(Green, HIGH);
  delay(100);
  digitalWrite(Red, HIGH);
  digitalWrite(Blue, LOW);
  digitalWrite(Green, HIGH);
  delay(100);
    digitalWrite(Red, HIGH);
  digitalWrite(Blue, LOW);
  digitalWrite(Green, LOW);
  delay(100);
  digitalWrite(Red, HIGH);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, LOW);
  delay(100);
  digitalWrite(Red, LOW);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, LOW);
  delay(100);  
    digitalWrite(Red, LOW);
  digitalWrite(Blue, LOW);
  digitalWrite(Green, LOW);
  delay(100);
}

