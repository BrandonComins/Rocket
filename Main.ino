#include <SparkFunMPL3115A2.h>
#include <Wire.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "SD.h"



  #define SDFILE_PIN_CS 10


  //altimeter  
  MPL3115A2 altimeter;
  float pressure;
  float temperature;
  int ipress;
  int itemp;

  char pastring[10];
  char tmpstring[10];

  //Accelerometer
  int scale = 3;
  boolean micro_is_5V = true;

  //SDd
  File sdFile;

void setup() 
{
    Wire.begin();        // Join i2c bus
  
    Serial.begin(9600);

    altimeter.begin(); // Get sensor online
    altimeter.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
    altimeter.setOversampleRate(7); // Set Oversample to the recommended 128
    altimeter.enableEventFlags(); // Enable all three pressure and temp event flags 

         
  
    pinMode(SDFILE_PIN_CS, OUTPUT);
    // Check if the card is present and can be initialized
    if (!SD.begin()) {
        Serial.println(F("Card failed, or not present"));
        while(1);
    }
    Serial.println(F("card initialized."));
    
}

void loop(){
  //constantly declaring the values for Accelerometer
    int rawX = analogRead(A0);
    int rawY = analogRead(A1);
    int rawZ = analogRead(A2); 
    float scaledX, scaledY, scaledZ;
     
  
  
   sdFile = SD.open("datalog.txt", FILE_WRITE);

    if (sdFile) {
        
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
  
      delay(500);
        
  //Altimeter
      pressure = altimeter.readPressure();
      ipress = pressure;
      sprintf(pastring, "%3d", ipress);

      sdFile.print(F("Pressure(Pa): "));
      sdFile.print(pastring);
    
      sdFile.println();
        
      temperature = altimeter.readTempF();
      itemp = temperature;
      sprintf(tmpstring, "%3d", itemp);  

      sdFile.print(F(" Temp(f): "));
      sdFile.print(temperature, 2);

      sdFile.println();

      delay(500);
      
    }else{
        // if the file didn't open, print an error
        Serial.println(F("error opening file."));   
    }
}

    float mapf(float x, float in_min, float in_max, float out_min, float out_max)
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }



 

    



