/*
   !!!!NOTE!!!!
   In order to use this library, you have to have at least ESP8266 Core version 2.5.0.
   If You have 2.4.2 and older, you must update ESP8266 Core.
   It will not work on 2.4.2. and older!
*/

#include "Maker_LED_Matrix_32.h"           //We include Maker LED Matrix Display library
Maker_LED_Matrix_32 led;                   //Maker LED Matrix display library constructor.

const char* wlan_ssid = "INSERT_YOUR_SSID_NAME_HERE";      //WLAN name network name
const char* wlan_pass = "INSERT_YOUR_WIFI_PASSWORD_HERE";         //WLAN network password
char scrollText[100];                       //String for text from web

void setup() {
  Serial.begin(9600);                                       //Setting up a serial comunication.

  if (!led.begin()) {
    Serial.println("Init fail!");      //If init has not gone well, there is something wrong with ESP8266 or one of LED matrix drivers.
    do {                               //In that case, stop the program forcing it into inf. loop.
      delay(1);
    } while (true);
  }

  if (!led.wifiNetwork(wlan_ssid, wlan_pass)) {   //Now let's try to connect to out Wifi.
    Serial.println("Connecting to WiFi failed!\nCheck WLAN SSID name and WLAN Password.");
    led.message("Can't connect to WiFi!", 120, 1, -1);
    do {                               //In that case, stop the program forcing it into inf. loop.
      delay(1);
    } while (true);
  } else {
    Serial.print("Connected to ");  Serial.println(wlan_ssid);
  }

  //Let's try to write some message on display.
  led.message("Trying to open web page...", 150, 3, 1);

  //While that message is scrolling, let's try to open a web page and display content from them on display. In this case todays Weather forecast for Croatia. :)
  led.webPage("http://api.thingspeak.com/apps/thinghttp/send_request?api_key=8X15NO5QU69EOGCJ", 150, 3, 1);

  do {
    delay(250);
  } while (!led.repeatCount());

  //And now, let' try to open web page and save it's content to string and then display it. Useful if parsing is needed before displaying.
  //We are saving tommorow Weather forecast for Croatia. String is limited to 99 characters.
  led.webPageText("http://api.thingspeak.com/apps/thinghttp/send_request?api_key=8B8OCD7Y2P6KXQK2", scrollText, 100);

  //Now let's print a new message on display, without scrolling.
  led.message(scrollText, 150, 2, 0);
  delay(4000);

  //And now we scroll it all the time.
  led.message(scrollText, 150, 2, -1);
}

void loop() {
  //Nothing...
}
