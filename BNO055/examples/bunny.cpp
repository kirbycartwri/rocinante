#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "BNO055.h"


/* 
   Connections to Raspberry Pi W:
   ==============================
   Connect SCL to pin 5
   Connect SDA to analog 3
   Connect VDD to 5V DC pin 2
   Connect GROUND to either of pins 6 or 9
   Connect RESET to pin 7 (GPIO 4)

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

  /* Board layout:
         +----------+
         |         *| RST   PITCH  ROLL  YAW(HEADING)
     ADR |*        *| SCL
     INT |*        *| SDA     ^            /->
     PS1 |*        *| GND     |            |
     PS0 |*        *| 3VO     Y    Z-->    \-X
         |         *| VIN
         +----------+
  */


/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

static BNO055 *mySensor = NULL;

static bool bContinue = true;

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{

  (void)printf("------------------------------------\n");
  (void)printf("Sensor:       \"%s\"\n", mySensor->getName());
  (void)printf("------------------------------------\n");

  (void)usleep(1000*500);
 
}

static void teardown(void)
{
  delete mySensor;
  mySensor = NULL;
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
static void setup(void)
{
  mySensor = new BNO055();

  // Zero the offsets of the sensor here.  
  mySensor->getOffsets();

  (void)printf("Orientation Sensor Test\n");
   
  (void)sleep(1);
   
  /* Display some basic information on this sensor */
  displaySensorDetails();

  bContinue = true;
 
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
static void loop(void)
{
    double_t pitch, roll, yaw;

    mySensor->readOrientation(pitch, roll, yaw); 

    /* The processing sketch expects data as pitch, roll, and yaw. */
    (void)printf("Orientation: %lf %lf %lf\n", pitch, roll, yaw);

    /* Also send calibration data for each sensor. */
    /*  This is an orphan. Remove.*/
    int32_t sys = 0, gyro = 0, accel = 0, mag = 0;

    (void)printf("Calibration: %i %i %i %i\n", sys, gyro, accel, mag);

    (void)usleep(1000*BNO055_SAMPLERATE_DELAY_MS);

}

int main(void)
{

  setup();

  double_t w = 0.0, x = 0.0, y = 0.0, z = 0.0;

  mySensor->readGyroscope(x, y, z);

  mySensor->readAccelerations(x, y, z);

  mySensor->readLinearAccelerations(x, y, z);

  mySensor->readGravities(x, y, z);

  mySensor->readCompass(x, y, z);    

  mySensor->readQuaternions(w, x, y, z);  

  mySensor->readOrientation(x, y, z);  


  while ( bContinue )
  {
    loop();
  }

  teardown();

  return 0;
}
