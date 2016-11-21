/****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// function prototypes
void connect(void);

/****************************** Pins ******************************************/

#define BUTTON          12
#define Blue            5  // LED Color
#define Green            4  // LED Color
#define Red            2  // LED Color

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
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);/****************************** Feeds ***************************************/

/****************************** Feeds ***************************************/
 
// Setup a feed called 'food-state' for subscribing to changes.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char AssistiveCallButtons_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-state";
Adafruit_MQTT_Subscribe AssistiveCallButtons = Adafruit_MQTT_Subscribe(&mqtt, AssistiveCallButtons_FEED);


// Setup a feed called 'food-state' for publishing changes.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char BUTTON_FEED[] PROGMEM = AIO_USERNAME "/feeds/food-state";
Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, BUTTON_FEED);

/*************************** Sketch Code ************************************/

// button state
int current = 0;
int last = -1;

void setup() {

  // set button pin as an input
  pinMode(BUTTON, INPUT_PULLUP);
  
  // set power switch tail pin as an output
  pinMode(Blue, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);

  digitalWrite(Red, HIGH);
  digitalWrite(Blue, HIGH);
  digitalWrite(Green, HIGH);

  Serial.begin(115200);

  Serial.println(F("Lunch Button"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Red, LOW);
    delay(500);
    digitalWrite(Red, HIGH);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // connect to adafruit io
  connect();

}

void loop() {

  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

  // grab the current state of the button
  current = digitalRead(BUTTON);

  // return if the value hasn't changed
  if(current == last)
    return;

  int32_t value = (current == LOW ? 1 : 0);

  // Now we can publish stuff!
  Serial.print(F("\nSending button value: "));
  Serial.print(value);
  Serial.print("... ");

  if (! button.publish(value))
    {
    Serial.println(F("Failed."));
    digitalWrite(Red,LOW);
    delay(500);
    digitalWrite(Red,HIGH);
    }
  else
    {
    Serial.println(F("Success!"));
    digitalWrite(Green, LOW);
    delay(500);
    digitalWrite(Green, HIGH);
    }
  // save the button state
  last = current;

}

// connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

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

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    digitalWrite(Red, LOW);
    delay(500);
    digitalWrite(Red, HIGH);
    delay(4500);

  }

  Serial.println(F("Adafruit IO Connected!"));

}
