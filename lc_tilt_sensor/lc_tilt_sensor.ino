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

#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

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

Adafruit_BNO055 bno = Adafruit_BNO055(55);

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

  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  bno.setExtCrystalUse(true);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  draw_front_img(frontlevel);
  draw_profile_img(profilelevel);

  draw_angle_text(0.0, 0.0);

  display.display();

}

void loop() {
  // put your main code here, to run repeatedly:
  sensors_event_t event;
  bno.getEvent(&event);

  display.clearDisplay();


  update_front_image((int) event.orientation.y)
  update_profile_image((int) event.orientation.z - 90.0);
  draw_angle_text(event.orientation.y, event.orientation.z - 90.0);
  display.display();
  delay(1000);


}




void update_front_image(int front_angle) {
  switch (front_angle) {
    case 0 ... 4:
      draw_front_img(frontlevel);
      break;
    case 5 ... 9:
      draw_front_img(front5d);
      break;
    case 10 ... 14:
      draw_front_img(front10d);
      break;
    case 15 ... 19:
      draw_front_img(front15d);
      break;
    case 20 ... 29:
      draw_front_img(front25d);
      break;
    case 30 ... 34:
      draw_front_img(front30d);
      break;
    case 35 ... 90:
      draw_front_img(front35d);
      break;
    case -4 ... -1:
      draw_front_img(frontlevel);
      break;
    case -9 ... -5:
      draw_front_img(front5p);
      break;
    case -14 ... -10:
      draw_front_img(front10p);
      break;
    case -19 ... -15:
      draw_front_img(front15p);
      break;
    case -29 ... -20:
      draw_front_img(front25p);
      break;
    case -34 ... -30:
      draw_front_img(front30p);
      break;
    case -90 ... -35:
      draw_front_img(front35p);
      break;
    default:
      draw_front_img(frontlevel);
      break;
  }

}

void update_profile_image(int profile_angle) {
  switch (profile_angle) {
    case 0 ... 4:
      draw_profile_img(profilelevel);
      break;
    case 5 ... 9:
      draw_profile_img(profile5f);
      break;
    case 10 ... 14:
      draw_profile_img(profile10f);
      break;
    case 15 ... 19:
      draw_profile_img(profile15f);
      break;
    case 20 ... 24:
      draw_profile_img(profile20f);
      break;
    case 25 ... 29:
      draw_profile_img(profile25f);
      break;
    case 30 ... 34:
      draw_profile_img(profile30f);
      break;
    case 35 ... 90:
      draw_profile_img(profile35f);
      break;
    case -4 ... -1:
      draw_profile_img(profilelevel);
      break;
    case -9 ... -5:
      draw_profile_img(profile5r);
      break;
    case -14 ... -10:
      draw_profile_img(profile10r);
      break;
    case -19 ... -15:
      draw_profile_img(profile15r);
      break;
    case -24 ... -20:
      draw_profile_img(profile20r);
      break;
    case -29 ... -25:
      draw_profile_img(profile25r);
      break;
    case -34 ... -30:
      draw_profile_img(profile30r);
      break;
    case -90 ... -35:
      draw_profile_img(profile35r);
      break;
    default:
      draw_profile_img(profilelevel);
      break;
  }

}

// draw the front image that is passed
void draw_front_img(const unsigned char bitmap[]) {
  //display.clearDisplay();

  display.drawBitmap(
    0,
    0,
    bitmap, LC_IMG_WIDTH, LC_IMG_HEIGHT, 1);

}

// draw the profile image that is passed
void draw_profile_img(const unsigned char bitmap[]) {
  //display.clearDisplay();

  display.drawBitmap(
    64,
    0,
    bitmap, LC_IMG_WIDTH, LC_IMG_HEIGHT, 1);

}

// draw the angle text below the image
void draw_angle_text(float front_angle, float profile_angle){

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(24, 56);     // Start under the images
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.print(front_angle, 1);
  display.write((char)248);

  display.setCursor(88, 56);

  display.print(profile_angle, 1);
  display.write((char)248);


}
