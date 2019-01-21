/*
  This library should be used with arduino IDE on e-radionica Social Display Module.
  
  Used librarys:
  Adafruit IS31FL3731 Library: https://github.com/adafruit/Adafruit_IS31FL3731
  Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
  Ticker Library for timer: https://github.com/esp8266/Arduino/tree/master/libraries/Ticker
*/

#ifndef _MAKER_LED_MATRIX_32_H_
#define _MAKER_LED_MATRIX_32_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define _BUFFERSIZE 800                        //Size of buffer for displying messages.
#define _GAMMA 2.2

static uint8_t _brightness;                    //Variables for font and background brightness and also for delay between shifts and shift (scroll) step.
static uint8_t _backBrightness;
static uint8_t _pause;
static uint8_t _step;

static char msgBuffer[_BUFFERSIZE];                    //Buffer for user defined message, max size of 799 characters.
static int _msgSize;                           //Variable that holds a size of user message.
static int _scroll;                            //Variable for scrolling text on display (defines maximal scroll steps for the whole message to be scrolled).
static int _position;                          //Variable that moves position of text on screen.
static uint8_t wlanSuccess = 0;                //Variable that holds information of WiFi connectivity.
static uint8_t* pictBuffer;                    //Pointer to user-defined picture (bitmap).
static int _repeats = 0;                       //Variable that keep number that says how many more times message have to be displayed.
static uint16_t _messageRepeats = 0;           //Variable that keeps track how many times message has been displayed (repeated).
static uint8_t** picTxtBuffer;                 //Variable for pointing on addresses of pictures
static uint16_t *posBufferX;                   //Variable for pointing on x cooridinates of each picture
static uint16_t *posBufferY;                   //Variable for pointing on y cooridinates of each picture
static uint8_t noOfPics;                       //Variable that keep number of pictures on screen.
static uint8_t _frameCount;                    //Variable that keeps track on what frame buffer is displayed on screen.


static void writeMessage();                    //Function that makes scrolling text on screen (called from timer).
static void writePicture();                    //Same as previous function, only for picture
static void writeTextAndPic();

class Maker_LED_Matrix_32 {
      public:
             Maker_LED_Matrix_32();
             int begin();                                          //Init function.
             int initLEDMatrix();                                  //Function sets up everything e-radionica Social Dispaly board.
             void brightness(uint8_t _fontLight, uint8_t _backingLight);      //Sets up font brightness and background brightness.
             void message(char* msg, int _ms, int _stp, int _rep); //Writes user defined message/text on display.
             void stopScroll();                                    //Calling this function scrolling will stop.
             void resumeScroll();                                  //Calling this function scrolling will continue.
             void deleteScroll();                                  //Function removes everything from screen and clears buffer.
             void picture(uint8_t* p, int posX, int posY);         //Function writes picture on screen (without scrolling).
             void scrollPicture(uint8_t* p, int _ms, int _stp);    //Function writes picture on screen with scrolling.
             void scrollTxtAndPics(char* txt, uint8_t** p, uint16_t* picsPos_x, uint16_t* picsPos_y, uint8_t n, int _ms, int _stp, int _rep);
             int repeatCount();                                    //Function return how much times message has been displayed
             void picture8Bit(uint8_t* p, int xSize, int ySize, int x0, int y0, uint8_t bright);    //Function that removes everything from screen and displays a 8bit gray scale image.
             
             int wifiNetwork(const char* _ssid, const char* _pass);                            //Funcion connects to WLAN network.
             int webPage(char* web, char* url, int port, int _ms, int _stp, int _rep);         //Function opens web page and displays text on LED matrix.
             int webPageText(char* web, char* url, int port, char* txt, int _n);               //Function opens a web and saves data into a string without displayig on matrix.
             
             int _dispMode;
};
#endif
