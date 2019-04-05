#include "Arduino.h"
#include "I2Cdev.h"
#include "SFE_BMP180.h"
#include "SoftwareSerial.h"
#include "SD.h"



// Pin Definitions
  #define SDFILE_PIN_CS 10


// Global variables and defines
  
  SFE_BMP180 altimeter;

  int scale = 3;
boolean micro_is_5V = true;

  File sdFile;

  double altimeterAltitude = altimeter.altitude();
  double altimeterPressure = altimeter.getPressure();
  double altimeterTempC = altimeter.getTemperatureC();

void setup() 
{
    Serial.begin(9600);
 
    altimeter.begin();
    
  
    pinMode(SDFILE_PIN_CS, OUTPUT);
    // Check if the card is present and can be initialized
    if (!SD.begin()) {
        Serial.println(F("Card failed, or not present"));
        while(1);
    }
    Serial.println(F("card initialized."));
    
}

void loop(){
  //constantly declaring the value for the pins
    int rawX = analogRead(A0);
    int rawY = analogRead(A1);
    int rawZ = analogRead(A2); 
    float scaledX, scaledY, scaledZ;
     
  
  
   sdFile = SD.open("datalog.txt", FILE_WRITE);

    if (sdFile) {
      writeAltimeter();
        
        if (micro_is_5V) // Microcontroller runs off 5V
  {
    scaledX = mapf(rawX, 0, 675, -scale, scale); // 3.3/5 * 1023 =~ 675
    scaledY = mapf(rawY, 0, 675, -scale, scale);
    scaledZ = mapf(rawZ, 0, 675, -scale, scale);
  }
  else // Microcontroller runs off 3.3V
  {
    scaledX = mapf(rawX, 0, 1023, -scale, scale);
    scaledY = mapf(rawY, 0, 1023, -scale, scale);
    scaledZ = mapf(rawZ, 0, 1023, -scale, scale);
  }
      // Print out raw X,Y,Z accelerometer readings
      sdFile.print(F("X: ")); sdFile.println(rawX);
      sdFile.print(F("Y: ")); sdFile.println(rawY);
      sdFile.print(F("Z: ")); sdFile.println(rawZ);
      sdFile.println();
  
  // Print out scaled X,Y,Z accelerometer readings
      sdFile.print(F("X: ")); sdFile.print(scaledX); sdFile.println(F(" g"));
      sdFile.print(F("Y: ")); sdFile.print(scaledY); sdFile.println(F(" g"));
      sdFile.print(F("Z: ")); sdFile.print(scaledZ); sdFile.println(F(" g"));
      sdFile.println();
    }else{
        // if the file didn't open, print an error
        Serial.println(F("error opening file."));   
    }
}


    void writeAltimeter(){
      sdFile.println(altimeterAltitude,1); sdFile.print(F(" [m]"));
      sdFile.println(altimeterPressure,1); sdFile.print(F(" [hPa]"));
      sdFile.println(altimeterTempC,1); sdFile.println(F(" [°C]"));
    }

    float mapf(float x, float in_min, float in_max, float out_min, float out_max)
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }



 

    



