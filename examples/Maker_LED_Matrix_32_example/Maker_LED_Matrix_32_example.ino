/*
   This is example code on how to use Maker LED Matrix Dispaly library on e-radionica Maker LED Matrix Display Board with ESP8266 and two IL31LF3731.
   It shows how to prite text/message to display, write picture, scroll it, pause it and remove it.
*/

#include "Maker_LED_Matrix_32.h"           //We include Maker LED Matrix Display library.
Maker_LED_Matrix_32 led;                   //Maker LED Matrix display library constructor.

unsigned int counter = 0;              //Counter that is used in main loop.
uint8_t  smile_bmp[] =                 //Bitmap pictures that can be displayed on matrix. Bitmap must be 8x8 pixels.
{
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

uint8_t neutral_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01000010,
  B00111100
};

/*For converting greyscale image, use LCD Image Converter.
 * Open program->New Image->Type Image Name and click ok->Image->Import
 * Then select image that you want to convert (note that image should not be bigger than dispaly. If it is, use Paint to resize it.
 * Options->Conversion
 * Preset should be on Grayscale 8, Type Grayscale, Inverse selected, main scan direction Top to bottom
 * Line scan direction Forward. Than click on Image, block size should be 8 bit and byte order to little endian.
 * By clicking Show Preview, you should see image as well as data array. Colors on image should be inversed.
 * Click ok, save it, then File->Convert and save your data array. Open it and copy here.
*/
uint8_t flame[288] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x69, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x93, 0x93, 0x00, 0x00, 0x00, 0x93, 0x69, 0x69, 0x93, 0x93, 0x93, 0x00, 0x93, 0x93, 0x69, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x93, 0x93, 0x69, 0x93, 0x93, 0x93, 0x93, 0x69, 0x69, 0x69, 0x69, 0x93, 0x93, 0x93, 0x69, 0x69, 0x93, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x93, 0x93, 0x93, 0x93, 0x69, 0x69, 0x3f, 0x69, 0x69, 0x69, 0x69, 0x3f, 0x3f, 0x69, 0x69, 0x69, 0x69, 0x3f, 0x69, 0x69, 0x93, 0x93, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x69, 0x69, 0x3f, 0x3f, 0x0f, 0x0f, 0x3f, 0x3f, 0x3f, 0x0f, 0x3f, 0x3f, 0x3f, 0x0f, 0x3f, 0x3f, 0x69, 0x69, 0x69, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x69, 0x69, 0x3f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x3f, 0x69, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t* listOfPics[] = {smile_bmp, neutral_bmp};     //Here you put pictures as described in order that you want to show. Display will first display smile_bmp picture then neutral_bmp.
uint16_t picPositionx[2] = {60, 139};                 //Put x cooridinates where you want to put picture. Also, it has to be in order, x = 60 is for first picture, x = 150 is for second picture and so on...
uint16_t picPositiony[2] = {0, 1};                    //Put y cooridinates where you want to put picture. Also, it has to be in order, x = 0 is for first picture, x = 1 is for second picture and so on...

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                         //Setting up a serial comunication.
  //ESP.wdtDisable();
  //Maker LED Matrix display library init.
  if (!led.begin()) {
    Serial.println("Init fail!");      //If init has not gone well, there is something wrong with ESP8266 or one of LED matrix drivers.
    do {                               //In that case, stop the program forcing it into inf. loop.
      delay(1);
    } while (true);
  }
}

void loop() {
  
  //For start, let's just write some static text (that means zero repeats), but still, you have to type some values for delay and step.
  led.message("Hello!", 100, 1, 0);
  delay(1000);                            //We wait a little bit, just to see the whole message.

  //Let's write message on LED matrix, with scroll step of one pixel and delay beetwen steps of 100 miliseconds.
  led.message("Hello world!", 100, 1, 2);
  delay(6000);                            //We wait a little bit, just to see the whole message.

  //Now we stop the message.
  led.stopScroll();
  delay(1000);                            //Again, wait a little bit.

  //After that, we continue with scrolling text (message).
  led.resumeScroll();
  delay(2500);
  
  //New message has been sent to display, with scroll step of three pixels and delay beetwen steps of 50 miliseconds.
  led.message("LED Matrix says:Hello World", 50, 3, 2);
  delay(5000);

  //And now, we delete everything.
  led.deleteScroll();
  delay(500);

  //Let's try to send a bitmap picture to display.
  led.picture(smile_bmp, 5, 0);
  delay(1500);
  
  //Now, let's make that same picture scroll from right to left, with four pixels step and delay of 250 miliseconds beetwen steps.
  led.scrollPicture(smile_bmp, 150, 6);
  delay(6000);
  Serial.println(counter, DEC);
  Serial.print("Message have been displayed ");
  Serial.print(led.repeatCount());
  Serial.println(" times");

  //Delete picture from display.
  led.deleteScroll();

  //Chagne font brightness to 128 and background brightness to 6.
  led.brightness(128, 6);

  //But what about text and picture together? 
  led.scrollTxtAndPics("First pic:  second pic:  ", listOfPics, picPositionx, picPositiony, 2, 100, 5, 2);

  //Wait until message with pictures is displayed exacly 2 times.
  do{
    delay(250);
  }while(led.repeatCount()<2);

  //You think thats all? Nope! It can also display 8 bit gray scale image! It also has Gamma correction included.
  //Clear all and display image of 32px x 9px at location 0,0 with brightness level of 120 of 255.
  led.brightness(128, 0);
  led.picture8Bit(flame, 32, 9, 0, 0, 120);
  
  delay(6000);
  led.brightness(128, 6);
  //And last thing, we write a new message to screen, that will be scrolled all the time, while the main program is running.
  led.message("Visit https://e-radionica.com/", 150, 2, -1);
  
  //Send seconds as the message is beening scrolled on display.
  do{
    Serial.println(counter, DEC);
    Serial.print("Message have been displayed ");
    Serial.print(led.repeatCount());
    Serial.println(" times");
    delay(1000);
    counter++;
  }while(true);
}
