#include "Maker_LED_Matrix_32.h"        //Header file of library.

#include <Wire.h>                 //Arduino Wire library.
#include <Adafruit_GFX.h>         //Adafruit GFX library.
#include <IS31FL3731.h>  //Library for IS31FL3731 LED Matrix Driver.
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

IS31FL3731 matrixL = IS31FL3731(); //LED Matrix IC on the left side of board.
IS31FL3731 matrixR = IS31FL3731(); //LED Matrix IC on the right side of board.
Ticker updateLED = Ticker();                                     //Constructor for timmer that refreshes display.

Maker_LED_Matrix_32::Maker_LED_Matrix_32() {
}

//--------------------------------------LOW LEVEL FUNCTIONS FOR DISPLAYING A MESSAGE--------------------------------------
void writeMessage() {
  //This part of code scrolls message on display (also sends data to display).
  if (_position < _scroll) {
     _position = 32;                                   //If text has come to end, bring it back to start.
     if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
     _messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
  }
  
  if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
    updateLED.detach();
    return;
  }
  
  //matrixL.setFrame((_frameCount&1)+1);               //Use FRAME 1 and FRAME 2 buffer in display for displaying images. We are using this method of double buffernig to reduce picure ficker.
  //matrixR.setFrame((_frameCount&1)+1);                //While we are writing in one buffer (frame), we are displaying second, and in next iterration, we are writing in second, and displaying first.
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
  matrixL.setCursor(_position, 0);            //Set cursor at the start of the display.
  matrixL.print(msgBuffer);                   //Send text to the left side of display (becouse display is made form 2 seperate LED matrix drivers that are not chained together).
  matrixR.setCursor(_position - 16, 0);       //Set cursor offseted by the size of first display driver (on the left side).
  matrixR.print(msgBuffer);                   //Send text to the right side of display.
  //matrixL.displayFrame((_frameCount&1)+1);    //Display data that is written into buffer.
  //matrixR.displayFrame((_frameCount&1)+1);
  matrixL.display();    //Display data that is written into buffer.
  matrixR.display();
  _position -= _step;                         //Increment counter for scrolling.
  _frameCount++;                              //Increment variable that keeps track which frame is displayed.
}

void writePicture () {
  if (_position < _scroll) {
    _position = 32;                                 //If picture has come to the end of screen, bring it back to start.
    _messageRepeats++;                              //And also, increment variable that keeps track of how many times picture has been repeated on display.
  }
  
  //matrixL.setFrame((_frameCount&1)+1);                //Use FRAME 1 and FRAME 2 buffer in display for displaying images. We are using this method of double buffernig to reduce picure ficker.
  //matrixR.setFrame((_frameCount&1)+1);                //While we are writing in one buffer (frame), we are displaying second, and in next iterration, we are writing in second, and displaying first.
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
  matrixL.drawBitmap(_position, 0, pictBuffer, 8, 8, _brightness);                 //Write picture to dispaly. Watchout! Picture that has to be written to right side of matrix should be shifted by the size of left LED matrix coltroler.
  matrixR.drawBitmap(_position - 16, 0, pictBuffer, 8, 8, _brightness);
  //matrixL.displayFrame((_frameCount&1)+1);                             //Display data that is written into buffer.
  //matrixR.displayFrame((_frameCount&1)+1);
  matrixL.display();                             //Display data that is written into buffer.
  matrixR.display();
  _frameCount++;                                                       //Increment variable that keeps track which frame is displayed.
  _position -= _step;                                                              //Increment counter for scrolling.                   
}

void writeTextAndPic() {
  if (_position < _scroll) {
     _position = 32;                                   //If text has come to end, bring it back to start.
     if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
     _messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
  }
  
  if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
    updateLED.detach();
    return;
  }
  
  //matrixLL.setFrame((_frameCount&1)+1);
  //matrixLR.setFrame((_frameCount&1)+1);
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
  matrixL.setCursor(_position, 0);            //Set cursor at the start of the display.
  matrixL.print(msgBuffer);                   //Send text to the left side of display (becouse display is made form 2 seperate LED matrix drivers that are not chained together).
  matrixR.setCursor(_position - 16, 0);       //Set cursor offseted by the size of first display driver (on the left side).
  matrixR.print(msgBuffer);                   //Send text to the right side of display.
  //matrixL.displayFrame((_frameCount&1)+1);
  //matrixR.displayFrame((_frameCount&1)+1);
  _frameCount++;

  for (int i = 0; i < noOfPics; i++) {
    matrixL.drawBitmap(_position + posBufferX[i], posBufferY[i], picTxtBuffer[i], 8, 8, _brightness);                       //Write picture to dispaly. Watchout! Picture that has to be written to right side of matrix should be shifted by the size of left LED matrix coltroler.
    matrixR.drawBitmap(_position - 16 + posBufferX[i], posBufferY[i], picTxtBuffer[i], 8, 8, _brightness);                  //And also shift it by position of the desiered location of picture.
  }

  matrixL.display();
  matrixR.display();
  _position -= _step;
}

//--------------------------------------FUNCTIONS FOR INIT--------------------------------------

int Maker_LED_Matrix_32::begin() {
    _brightness = 32;                             //Set default levels for font brightness, background brightness, delay between scrolls and scroll step.
    _backBrightness = 2;
    _pause = 150;
    _step = 2;
    return initLEDMatrix();                      //Set up everything for our board and return value (1 - Success, 0- Fail).
}

int Maker_LED_Matrix_32::initLEDMatrix() {
  Wire.begin(4, 5);                              //Set up SDA ands SCL Pins.
  twi_setClock(400000);                          //Set maximum possible clock for this LED controller (400kHz).      
  if (!matrixL.begin(0x74)) return 0; //Initalize library for left LED matrix driver.
  if (!matrixR.begin(0x77)) return 0; //Initalize library for right LED matrix driver.
  //matrixLL.clear();                    //Clear everything that is in RAM od LED matrix Driver.
  //matrixLR.clear();
  matrixR.setTextWrap(false);         //Turn of text wrapin, we do not need that on this screen.
  matrixL.setTextWrap(false);
  matrixL.setTextColor(_brightness, _backBrightness);   //Setup font and background brightness.
  matrixR.setTextColor(_brightness, _backBrightness);
  matrixL.setRotation(0);                               //Set rotation to display messages and pictures horizontaly.
  matrixR.setRotation(0);
  return 1;                                             //Everything went ok? Return 1 for success!.      
}

void Maker_LED_Matrix_32::brightness(uint8_t _fontLight, uint8_t _backingLight) {
    _brightness = _fontLight;                               //If user want to change brightness of screen, they can use this function.
    _backBrightness = _backingLight;
    matrixL.setTextColor(_brightness, _backBrightness);
    matrixR.setTextColor(_brightness, _backBrightness);
}

//--------------------------------------FUNCTION FOR WRITING MESSAGE--------------------------------------
void Maker_LED_Matrix_32::message(char* msg, int _ms, int _stp, int _rep) {
  if(_ms < 75 ) _ms = 75;
  if(_stp < 1 ) _stp = 1;
  if(_rep < -1) _rep = -1;//It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, set preset values.
  if(strlen(msg) > _BUFFERSIZE) return;   //If size of string is much bigger of size of buffer, do not do anything more, it will overflow.
  _step = _stp;                                   //Save what user want for delay and step for scrolling.
  _pause = _ms;
  _repeats = _rep;
  _dispMode = 1;
  _frameCount = 0;
  
  
  _messageRepeats = 0;                //If there is a new message, reset counter!
  _position = 32;                     //Calling this function means that user whats to write some new message, so bring scroll counter to start and clear buffer for message.
  
  for (int i = 0; i < _BUFFERSIZE; i++) {       //Clean up whole message buffer.
    msgBuffer[i] = 0;
  }

  memcpy(msgBuffer, msg, strlen(msg));    //Copy new message to the buffer.
  _msgSize = strlen(msgBuffer);           //Calculate how many letters there is in new message (size of message).
  _scroll = ((-6) * _msgSize) - 1;        //Calculate how many steps of scrolling we have to make to scroll out whole message.
  

  updateLED.detach();                                 //Remove timer, just in case.
  
  if(_repeats) {                                        //If number of repeats is zero, that means that text is not scrolling, do not update screen.
    updateLED.attach_ms(_pause, writeMessage);          //Setup timer with new parameters for new message.
  }
  //matrixL.drawLine(0, 8, 15, 8, _backBrightness);     //Draw line at the end of screen. This is because Adafruit library uses 8x6 pixels for fonts and height of out display is 9 pixels.
  //matrixR.drawLine(0, 8, 15, 8, _backBrightness);
  if(!_repeats) {                                     //If repeats are equal to zero, that means that we do not want to scroll it, we just want to print message with picture.
    _repeats = 1;
    _position = 0;                                    //In that case, set position to start of the screen.
    writeMessage();
  }
}

void Maker_LED_Matrix_32::stopScroll() {        //Function that stops message from scrolling (just remove timmer).
  updateLED.detach();
}

void Maker_LED_Matrix_32::resumeScroll() {         //Function that resume scrolling message on the screen.         
  if(_dispMode == 1) updateLED.attach_ms(_pause, writeMessage);
  if(_dispMode == 2) updateLED.attach_ms(_pause, writePicture);
  if(_dispMode == 3) updateLED.attach_ms(_pause, writeTextAndPic);
}

void Maker_LED_Matrix_32::deleteScroll() {                //Function removes message form display and memory.
  updateLED.detach();                                //Remove timmer (do not refresh display anymore).
    if(_dispMode == 1 || _dispMode == 3) {           //If it's mode 1 od 3 that means that we need to delete some text.
    for (int i = 0; i < 500; i++) {                  //Clean up buffer.
      msgBuffer[i] = 0;
    }
  }
  
  if(_dispMode == 2 || _dispMode == 3) {         //But, if we are in mode 2 or 3, that means we have some pictures to delete.
    pictBuffer = NULL;
    noOfPics = 0;
  }
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);    //Delete everything from screen by writing filled rect on screen that has same intensity as background.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
}

int Maker_LED_Matrix_32::repeatCount() {                  //Function retrun how many times message has been displayed and repeated. Useful when user want to know that message is displayed till the end.
  return _messageRepeats;  
}

//---------------------------------------FUNCTION FOR DISPLAYING PICTURES--------------------------------------

void Maker_LED_Matrix_32::picture(uint8_t* p, int posX, int posY) {
  _frameCount = 0;                                             //Reset variable that keeps track on which frame (in LED Matrix IC) is currently displayed.
  matrixL.drawBitmap(posX, posY, p, 8, 8, _brightness);        //Write picture on screen (keeping on mind that picture has to be written on both LED Matrix drivers with offset on one).
  matrixR.drawBitmap(posX - 16, posY, p, 8, 8, _brightness);
}

void Maker_LED_Matrix_32::scrollPicture(uint8_t* p, int _ms, int _stp) {
  if(_ms < 75) _ms = 75;                             //It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, set preset values.
  if(_stp < 1) _stp = 1;
  
  int xSize = 8;                                     //Picture that has to be written on screen has to be 8x8 pixels.
  _dispMode = 2;
  _step = _stp;                                      //Save value for scroll step and delay between steps.
  _pause = _ms;

  _messageRepeats = 0;                               //If there is a new picture, reset counter!
  pictBuffer = p;                                    //Save picture address.
  _position = 64;                                //Set variable for current position on screen at start of screen.
  _scroll = -xSize - _step;
  updateLED.detach();                            //Remove timer, just in case.
  updateLED.attach_ms(_pause, writePicture);     //Setup timer with new parameters for new picture.
}


//--------------------------------------FUNCTIONS FOR DISPLAYING TEXT AND PICTURES AT THE SAME TIME--------------------------------------
void Maker_LED_Matrix_32::scrollTxtAndPics(char* txt, uint8_t** p, uint16_t* picsPos_x, uint16_t* picsPos_y, uint8_t n, int _ms, int _stp, int _rep) {
  if(_ms < 75) _ms = 75;                    //It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, set preset values.
  if(_stp < 1 ) _stp = 1;
  if(_rep < -1) _rep = -1;
  if(n<1) return;
  
  if(strlen(txt) > _BUFFERSIZE) return;   //If size of string is much bigger of size of buffer, do not do anything more, it will overflow.
  _step = _stp;                                            //Save what user want for delay and step for scrolling.
  _pause = _ms;
  _repeats = _rep;
  _dispMode = 3;                                           //Set mode for displaying message and thex at the same time.     
  _messageRepeats = 0;                                     //If there is a new message, reset counter!
  int16_t _max;                                            //Variable that we are using for calculation of maximal scroll position.
  _frameCount = 0;

  _position = 32;                     //Calling this function means that user whats to write some new message, so bring scroll counter to start and clear buffer for message.
  for (int i = 0; i < _BUFFERSIZE; i++) {
    msgBuffer[i] = 0;
  }

  memcpy(msgBuffer, txt, strlen(txt));    //Copy new message to the buffer.
  _msgSize = strlen(msgBuffer);           //Calculate how many letters there is in new message (size of message).
  _scroll = ((-6) * _msgSize);            //Calculate how many steps of scrolling we have to make to scroll out whole message.

  _max = _scroll;                         //Now, we are trying to fing what needs longer scrolling, text or picture.

  for (int i = 0; i < n; i++) {
    if (-int16_t((picsPos_x[i] + 16)) < _max) _max = -int16_t((picsPos_x[i]) + 16);      //And how much exactly we have to scroll.
  }

  picTxtBuffer = p;                        //Just copy everything in library variables.
  posBufferX = picsPos_x;
  posBufferY = picsPos_y;
  noOfPics = n;                            //Determines how may pictures are in this text.
  _scroll = _max;

  updateLED.detach();                                      //Remove timer, just in case.
  if(_repeats) {                                           //If number of repeats is zero, that means that text is not scrolling, do not update screen.
    updateLED.attach_ms(_pause, writeTextAndPic);          //Setup timer with new parameters for new message.
  }
  
  if(!_repeats) {                                          //If repeats are equal to zero, that means that we do not want to scroll it, we just want to print message with picture.
    _repeats = 1;
    _position = 0;                                         //In that case, set position to start of the screen.
    writeTextAndPic();
  }
}

//--------------------------------------FUNCTION FOR DISPLAYING GREYSCALE WTIH GAMMA CORRECTION-----------------
void Maker_LED_Matrix_32::picture8Bit(uint8_t* p, int xSize, int ySize, int x0, int y0, uint8_t bright) {
  updateLED.detach();                                //For now, we can display only image on screen, so we have to remove everything on screen and block future updates.
  //matrixL.setFrame(3);                               //Set a new frame on screen. Basicly this is a memory place where we will write our data and after we wrote all our data, we can display it without flickering.
  //matrixR.setFrame(3);                               //Do than on both LED Matrix ICs
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);    //Delete everything from screen by writing filled rect on screen that has same intensity as background.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
  int endPosX = xSize + x0;                          //Calculate end position for x cooridinate (needed for x position offest).

  int n = 0;                                         //Variable for indexing picture array
  for (int j = 0; j < ySize; j++) {                  //First select row, then write data into seleced row
    for (int i = 0+x0; i < endPosX; i++) {
      if (i > 0 && i < 16) matrixL.drawPixel(i, j + y0, (uint8_t)(bright * pow(p[n] / 255.0, _GAMMA)));     //Otherwise write picure data into first (left) Matrix IC
	  if (i > 15 && i < 32) matrixR.drawPixel(i - 16, j + y0, (uint8_t)(bright * pow(p[n] / 255.0, _GAMMA)));
      n++;                   //Increment variable for array indexing
    }
  }

  //matrixL.displayFrame(3);   //Done writing data? Nice, now display that! :)
  //matrixR.displayFrame(3);
  matrixL.display();
  matrixR.display();
}

//--------------------------------------FUNCTIONS FOR WEB--------------------------------------
int Maker_LED_Matrix_32::wifiNetwork(const char* _ssid, const char* _pass) {
    int retry = 20;                              //Number of retrys for connectin on WLAN network.
    WiFi.mode(WIFI_STA);                         //Setting up WiFi module mode.
    WiFi.begin(_ssid, _pass);                    //Try to connect to desiered network.
    do {                                         //Wait and keep checking if it's connected to network until we are really connected or we run out of retrys
       delay(500);
       retry--;
       } while (WiFi.status() != WL_CONNECTED && retry != 0);
    if(retry) {                                  //If we still have any more retrys left, that means we are successfuly connected to WiFi network.
       wlanSuccess = 1;
       return 1;                                 //Return 1 for successful conection to WiFi.
    }else{
       return 0;                                 //But if we are not connected, return 0 fro fail.
    }
}

int Maker_LED_Matrix_32::webPage(char* url,  int _ms, int _stp, int _rep) {
  if (!wlanSuccess) return 0;                          //If we are not connected to WiFi, return 0, becouse we can't loar web page if we do not have at least WiFi connection.
  if(_ms < 75 || _stp < 1) return 0;                   //It doesn't make any sense to make delay less than 75 ms, especially negative one and step smaller than 1, so if that happens, return.
  
  WiFiClient client;
  HTTPClient http;
  if (http.begin(client, url)) {
    int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) {
			String payload = http.getString();
			char webText[_BUFFERSIZE];
          	payload.toCharArray(webText, payload.length());
          	int webTextSize = strlen(webText);                                 //Calculate the lenght of string.
    		
			//This part of code finds Croatian letters and change them into ASCII letters. Not really nesscery for this library.
    		for (int16_t i = 0; i < webTextSize; i++) {
      			if (webText[i] == 196 || webText[i] == 197) memmove(&webText[i], &webText[i + 1], webTextSize - i);
      			if (webText[i] == 141 || webText[i] == 135) webText[i] = 'c';
      			if (webText[i] == 161) webText[i] = 's';
      			if (webText[i] == 190) webText[i] = 'z';
    		}
    		message(webText, _ms, _stp, _rep);        //Send message to display with desiered delay and step for scrolling
        } else {
        	message("Failed to open web", _ms, _stp, -1); 
          	return 0;
        }
        http.end();
    }
    return 1;                           //Everything went ok? Return 1 for success!
}

int Maker_LED_Matrix_32::webPageText(char* url, char* webText, int _n) {  //This function grabs textz form web page and saves it in user defined string that is send as function argument.
  if (!wlanSuccess) return 0;                                                         //Everything else is basicly the same as previous function, with only one exception, it does not write anything on dipslay. Useful if we want to parse something and than display it.
  
  WiFiClient client;
  HTTPClient http;
  if (http.begin(client, url)) {
    int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) {
			String payload = http.getString();
          	payload.toCharArray(webText, _n);
          	int webTextSize = strlen(webText);                                 //Calculate the lenght of string.
    		
			//This part of code finds Croatian letters and change them into ASCII letters. Not really nesscery for this library.
    		for (int16_t i = 0; i < _n; i++) {
      			if (webText[i] == 196 || webText[i] == 197) memmove(&webText[i], &webText[i + 1], _n - i);
      			if (webText[i] == 141 || webText[i] == 135) webText[i] = 'c';
      			if (webText[i] == 161) webText[i] = 's';
      			if (webText[i] == 190) webText[i] = 'z';
    		}
        } else {
          	return 0;
        }
        http.end();
    }
    return 1;                           //Everything went ok? Return 1 for success!
}
