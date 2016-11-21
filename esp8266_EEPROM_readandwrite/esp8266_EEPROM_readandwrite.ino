/****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <EEPROM.h>

// function prototypes
void connect(void);
void battery_level();
void door_open();

/****************************** Pins ******************************************/

#define BUTTON          12
#define Blue            5  // LED Color
#define Green            4  // LED Color
#define Red            2  // LED Color

/****************************** Timings****************************************/

// how often to report battery level to adafruit IO (in minutes)
#define BATTERY_INTERVAL 15
// how long to sleep between checking the switch state (in seconds)
#define SLEEP_LENGTH 15

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Paris_2.4"
#define WLAN_PASS       "Rh3ttAndP3rcy"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "matthewf01"
#define AIO_KEY         "3d4eb4b0df734139a771f6289b1461f9"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

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

/****************************** Feeds ***************************************/
 
// Setup a feed called 'food-state' for subscribing to changes.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char AssistiveCallButtons_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-state";
Adafruit_MQTT_Subscribe AssistiveCallButtons = Adafruit_MQTT_Subscribe(&mqtt, AssistiveCallButtons_FEED);

/****************************** Feeds ***************************************/
 
//SUBSCRIBE FEEDS 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char FoodState_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-state";
Adafruit_MQTT_Subscribe foodstate = Adafruit_MQTT_Subscribe(&mqtt, FoodState_FEED);

//PUBLISH FEEDS 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char BUTTON_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-state";
Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, BUTTON_FEED);

const char Battery_FEED[] PROGMEM = AIO_USERNAME "/feeds/battery_level";
Adafruit_MQTT_Publish battery = Adafruit_MQTT_Publish(&mqtt, Battery_FEED);

/****************************** Sketch Code************************************/

int addrLastSwitchState = 1;
int addrBattState = 120;

void setup() {

  // set button pin as an input
  pinMode(BUTTON, INPUT_PULLUP);

  Serial.begin(115200);
  EEPROM.begin(512);
    
  Serial.println(F("test"));
  
  int valLastSwitchState = (EEPROM.read(addrLastSwitchState));
  Serial.print("Last state logged was ");
  Serial.println(valLastSwitchState);
  int valCurrentSwitchState = digitalRead(BUTTON);
  Serial.print("Current state is ");
  Serial.println(valCurrentSwitchState);
  
  if(valCurrentSwitchState != valLastSwitchState)
    {
   EEPROM.write(addrLastSwitchState, valCurrentSwitchState);
   EEPROM.commit();
   Serial.println("Stored the new value in EEPROM.");
    }
  else
    {
    Serial.println("The switch hasn't changed");
    return;
    }
 
  delay(100);
/******  
    // grab the current state of the button
  current = digitalRead(BUTTON);

  last == EEPROM.Read
  // return if the value hasn't changed


  int32_t value = (current == LOW ? 1 : 0);

  ***/
  
  
  }

void loop() 
{
}

void 
