/****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <EEPROM.h>

// function prototypes
void connect();
void battery_level();
void switch_change();
void rainbow_flash();

/****************************** Pins ******************************************/

#define BUTTON          12
#define Blue            5  // LED Color
#define Green            4  // LED Color
#define Red           13  //2  // LED Color

/****************************** Timings****************************************/

// how often to report battery level to adafruit IO (in minutes)
#define BATTERY_INTERVAL 20
// how long to sleep between checking the switch state (in seconds)
#define SLEEP_LENGTH 6

/************************* WiFi Access Point *********************************/

//office
#define WLAN_SSID       "KIDSII_Guest"
#define WLAN_PASS       "YUhdyk23Ut"

/*
IPAddress ip(192,168,100,251);  //Node static IP
IPAddress gateway(192,168,100,1);
IPAddress subnet(255,255,255,0);
*/

// mobile hotspot
//#define WLAN_SSID       "m"
//#define WLAN_PASS       "matthew123"


/************************* Adafruit.io Setup *********************************/

//#define AIO_SERVER      "52.70.203.194"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "matthewf01"
#define AIO_KEY         "405bdaf5f75349cd939765104fb18f4c"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

/*
// Store the MQTT server, client ID, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
// Set a unique MQTT client ID using the AIO key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ AIO_USERNAME;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);
*/
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


/****************************** Feeds ***************************************/

// Setup a feed called 'food-state' for subscribing to changes.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

Adafruit_MQTT_Subscribe AssistiveCallButtons = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/food-state");


/****************************** Feeds ***************************************/
/**
  //SUBSCRIBE FEEDS
  // Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
  const char FoodState_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-state";
  Adafruit_MQTT_Subscribe foodstate = Adafruit_MQTT_Subscribe(&mqtt, FoodState_FEED);
**/

//PUBLISH FEEDS
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//const char BUTTON_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-state";
//Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, BUTTON_FEED);
Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/food-state");

//const char BATTERY_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-button-battery-level";
//Adafruit_MQTT_Publish battery = Adafruit_MQTT_Publish(&mqtt, BATTERY_FEED);
Adafruit_MQTT_Publish battery = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/food-button-battery-level");

/****************************** Sketch Code************************************/


int addrLastSwitchState = 5;
int addrBattState = 125;
int valLastSwitchState = (EEPROM.read(addrLastSwitchState));
int valCurrentSwitchState = digitalRead(BUTTON);
int32_t FoodNoFood = (valCurrentSwitchState == LOW ? 1 : 0);

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

  int valLastSwitchState = (EEPROM.read(addrLastSwitchState));
  Serial.print("Last state logged was ");
  Serial.println(valLastSwitchState);
  int valCurrentSwitchState = digitalRead(BUTTON);
  Serial.print("Current state is ");
  Serial.println(valCurrentSwitchState);

  if (valCurrentSwitchState != valLastSwitchState)
  {
    EEPROM.write(addrLastSwitchState, valCurrentSwitchState);
    EEPROM.commit();
    Serial.println("Stored the new value in EEPROM. Will publish online.");
    switch_change();
    if (FoodNoFood = '1') {
      rainbow_flash();
    }
    if (FoodNoFood = '0') {
      digitalWrite(Red, HIGH);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, LOW);
      delay(500);
      digitalWrite(Red, HIGH);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, HIGH);
      }
  }
  else
  {
    Serial.println("The switch hasn't changed");
  }

  delay(100);

  // get the current count position from eeprom
  byte battery_count = EEPROM.read(addrBattState);

  // we only need this to happen once every X minutes,
  // so we use eeprom to track the count between resets.
  if (battery_count >= ((BATTERY_INTERVAL * 60) / SLEEP_LENGTH)) {
    // reset counter
    battery_count = 0;
    // report battery level to Adafruit IO
    battery_level();
  } else {
    // increment counter
    battery_count++;
    Serial.println("Not time to publish battery. Added to the tally.");
  }

  // save the current count
  EEPROM.write(addrBattState, battery_count);
  EEPROM.commit();

  // we are done here. go back to sleep.
  Serial.println("--going to sleep now--");
  ESP.deepSleep(SLEEP_LENGTH * 1000000, WAKE_RF_DEFAULT);
}

// connect to adafruit io via MQTT
void connect() {

  // wifi init
  Serial.println("Starting WiFi");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
//  WiFi.config(ip, gateway, subnet);
 
  // wait for connection
  while (WiFi.status() != WL_CONNECTED) {
      //flash blue lights when connecting
      digitalWrite(Red, HIGH);
      digitalWrite(Blue, LOW);
      digitalWrite(Green, HIGH);
      delay(80);
      digitalWrite(Red, HIGH);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, HIGH);
    delay(200);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
  yield();
  
  int8_t ret;

  Serial.println(F("Connecting to Adafruit IO... "));
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
      
    }
    if (ret >= 0)
      mqtt.disconnect();
    Serial.println(F("Retrying connection..."));
      digitalWrite(Red, LOW);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, HIGH);
      delay(80);
      digitalWrite(Red, HIGH);
      digitalWrite(Blue, HIGH);
      digitalWrite(Green, HIGH);
    yield();
    delay(5000);
  }
  Serial.println(F("Adafruit IO Connected!"));
  delay(100);
  //flash LED purple when connected to Adafruit
  digitalWrite(Red, LOW);
  digitalWrite(Blue, LOW);
  digitalWrite(Green, HIGH);
  delay(100);
  digitalWrite(Red, HIGH);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, HIGH);
  
}

void switch_change() {
  // turn on wifi if we aren't connected
  if (WiFi.status() != WL_CONNECTED) {
    connect();
  }
  // Now we can publish stuff!
  Serial.print(F("\nSending switch value: "));
  Serial.print(valCurrentSwitchState);
  Serial.print("... ");

  if (! button.publish(valCurrentSwitchState)) {
    Serial.println(F("Failed."));
    digitalWrite(Red, LOW);
    digitalWrite(Blue, HIGH);
    digitalWrite(Green, HIGH);
    delay(1000);
    digitalWrite(Red, HIGH);
    digitalWrite(Blue, HIGH);
    digitalWrite(Green, HIGH);
  }
  else {
      Serial.println(F("Success!"));
    }
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
}


void battery_level() {
  // read the battery level from the ESP8266 analog in pin.
  // analog read level is 10 bit 0-1023 (0V-1V).
  // our 1M & 220K voltage divider takes the max
  // lipo value of 4.2V and drops it to 0.758V max.
  // this means our min analog read value should be 580 (3.14V)
  // and the max analog read value should be 774 (4.2V).
  int battlevel = analogRead(A0);
  Serial.print("Measured raw battery value of ");Serial.println(battlevel);

  // convert battery level to percent
  battlevel = map(battlevel, 508, 702, 0, 100);
  Serial.print("Battery level: "); Serial.print(battlevel); Serial.println("%");

  // turn on wifi if we aren't connected
  if (WiFi.status() != WL_CONNECTED) {
    connect();
  }
  // Now we can publish stuff!
  Serial.print(F("\nSending battery level value: "));
  Serial.print(battlevel);
  Serial.print("... ");

  if (! battery.publish(battlevel)) {
    Serial.println(F("Failed."));
    digitalWrite(Red, LOW);
    digitalWrite(Blue, HIGH);
    digitalWrite(Green, HIGH);
    delay(1000);
    digitalWrite(Red, HIGH);
    digitalWrite(Blue, HIGH);
    digitalWrite(Green, HIGH);
  }
  else {
    //flash LED purple when connected to Adafruit
    digitalWrite(Red, LOW);
    digitalWrite(Blue, LOW);
    digitalWrite(Green, HIGH);
    delay(100);
    digitalWrite(Red, HIGH);
    digitalWrite(Blue, HIGH);
    digitalWrite(Green, HIGH);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
  }
}

void rainbow_flash() {
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
  digitalWrite(Red, HIGH);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, HIGH);
}

void loop()
{
}

