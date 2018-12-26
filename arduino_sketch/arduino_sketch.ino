/**

This code uses a rotation gyro to display an SUV's current tilt as an image
on a Adafruit OLED display. It mimics a JDM FJ60 tilt sensor.


Daniel Wiseman
dan@daninthe.cloud


**/




#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "lcbitmaps.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


#define LC_IMG_HEIGHT 64
#define LC_IMG_WIDTH  64





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  draw_front_img(frontlevel);
  draw_profile_img(profilelevel);

  display.display();

}

void loop() {
  // put your main code here, to run repeatedly:

}

int get_current_front_angle() {


  return 5;
}

int get_current_profile_angle() {
  return 10;
}

// determines the image to use based on the angle sent
const unsigned char angled_front_bitmap(int angle) {
  switch (angle) {
  case 0 ... 4:
    return frontlevel;
    break;
  case 5 ... 9:
    return front5d;
    break;
  case 10 ... 14:
    return front10d;
    break;
  case 15 ... 19:
    return front15d;
    break;
  case 20 ... 24:
    return front20d;
    break;
  case 25 ... 29:
    return front25d;
    break;
  case 30 ... 34:
    return front30d;
    break;
  case 35 ... 90:
    return front35d;
    break;

  default:
    return frontlevel;
    break;
}


}


// draw the bitmaps
void draw_front_img(const unsigned char bitmap[]) {
  //display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LC_IMG_WIDTH ) / 2,
    (display.height() - LC_IMG_HEIGHT) / 2,
    bitmap, LC_IMG_WIDTH, LC_IMG_HEIGHT, 1);

}


void draw_profile_img(const unsigned char bitmap[]) {
  //display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LC_IMG_WIDTH ) / 2 + 64,
    (display.height() - LC_IMG_HEIGHT) / 2 + 64,
    bitmap, LC_IMG_WIDTH, LC_IMG_HEIGHT, 1);

}

void draw_angle_text(int front_angle, int profile_angle){
  
}
