/**

This code uses a rotation gyro to display an SUV's current tilt as an image
on a Adafruit OLED display. It mimics a JDM FJ60 tilt sensor.


Daniel Wiseman
dan@daninthe.cloud


**/




#include <SPI.h>
#include <Wire.h>

// For the OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// For the Gyro
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>



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


/* Assign a unique ID to this sensor at the same time */
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
const float pi = 3.14159267;


inline unsigned long get_last_time() {return last_read_time;}
inline float get_last_x_angle() {return last_x_angle;}
inline float get_last_y_angle() {return last_y_angle;}
inline float get_last_z_angle() {return last_z_angle;}
inline float get_last_gyro_x_angle() {return last_gyro_x_angle;}
inline float get_last_gyro_y_angle() {return last_gyro_y_angle;}
inline float get_last_gyro_z_angle() {return last_gyro_z_angle;}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // gyro
  /* Enable auto-ranging */
  gyro.enableAutoRange(true);

  /* Initialise the sensor */
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
    while(1);
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  draw_front_img(frontlevel);
  draw_profile_img(profilelevel);

  draw_angle_text(0, 0);

  display.display();

}

void loop() {
  // put your main code here, to run repeatedly:
  /* Get a new sensor event */
  sensors_event_t event;
  gyro.getEvent(&event);

  /* Display the results (speed is measured in rad/s) */
  display.clearDisplay();

  draw_front_img(frontlevel);
  draw_profile_img(profilelevel);

  draw_angle_text((int)degrees(event.gyro.x), (int)degrees(event.gyro.y));
  Serial.print("X: "); Serial.print((int)convertDegrees(event.gyro.x)); Serial.print("  ");
  Serial.print("Y: "); Serial.print((int)convertDegrees(event.gyro.y)); Serial.print("  ");
  display.display();

  delay(2000); // Pause for 2 seconds
}

float convertDegrees(float radiansToCon) {
  return radiansToCon / 2 / pi * 360;
}


// from: https://hester.mtholyoke.edu/idesign/SensorTilt.html
void set_last_read_angle_data(unsigned long time, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro) {
  last_read_time = time;
  last_x_angle = x;
  last_y_angle = y;
  last_z_angle = z;
  last_gyro_x_angle = x_gyro;
  last_gyro_y_angle = y_gyro;
  last_gyro_z_angle = z_gyro;
}


// from: https://hester.mtholyoke.edu/idesign/SensorTilt.html
void initTiltSensor()
{
  set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0);
}

// based on https://hester.mtholyoke.edu/idesign/SensorTilt.html
void readTiltData()
{
  int error;
  double dT;
  accel_t_gyro_union accel_t_gyro;


  // Get the time of reading for rotation computations
  unsigned long t_now = millis();

  // Convert gyro values to degrees/sec
  float FS_SEL = 131;
  float gyro_x = (event.gyro.x - base_x_gyro)/FS_SEL;
  float gyro_y = (event.gyro.y - base_y_gyro)/FS_SEL;
  float gyro_z = (event.gyro.z - base_z_gyro)/FS_SEL;


  // Get raw acceleration values
  //float G_CONVERT = 16384;
  float accel_x = accel_t_gyro.value.x_accel;
  float accel_y = accel_t_gyro.value.y_accel;
  float accel_z = accel_t_gyro.value.z_accel;

  // Get angle values from accelerometer
  float RADIANS_TO_DEGREES = 180/3.14159;
//  float accel_vector_length = sqrt(pow(accel_x,2) + pow(accel_y,2) + pow(accel_z,2));
  float accel_angle_y = atan(-1*accel_x/sqrt(pow(accel_y,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
  float accel_angle_x = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;

  float accel_angle_z = 0;

  // Compute the (filtered) gyro angles
  float dt =(t_now - get_last_time())/1000.0;
  float gyro_angle_x = gyro_x*dt + get_last_x_angle();
  float gyro_angle_y = gyro_y*dt + get_last_y_angle();
  float gyro_angle_z = gyro_z*dt + get_last_z_angle();

  // Compute the drifting gyro angles
  float unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
  float unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
  float unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();

  // Apply the complementary filter to figure out the change in angle - choice of alpha is
  // estimated now.  Alpha depends on the sampling rate...
  float alpha = 0.96;
  float angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
  float angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
  float angle_z = gyro_angle_z;  //Accelerometer doesn't give z-angle

  // Update the saved data with the latest values
  set_last_read_angle_data(t_now, angle_x, angle_y, angle_z, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);

  // save in global variables
  xRotation = angle_x;
  yRotation = angle_y;
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
void draw_angle_text(int front_angle, int profile_angle){

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(24, 56);     // Start under the images
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.print(front_angle);
  display.write((char)248);

  display.setCursor(88, 56);

  display.print(profile_angle);
  display.write((char)248);


}
