#include <SD.h>
#include <SPI.h>
#include <SparkFunMPL3115A2.h>
#include <Wire.h>
#include "Arduino.h"
#include "SoftwareSerial.h"


  //altimeter  
  MPL3115A2 altimeter;
  float pressure;
  float temperature;
  float altitude;
  float currentAltitude;
  float previousAltitude;
  int ipress;
  int itemp;
  int temp;
  int alt;

  char pastring[10];
  char tmpstring[10];

  //Accelerometer
  int scale = 3;
  boolean micro_is_5V = true;

  //SD Card Reader
  File file;
  char fileName[] = "data.txt"; // SD library only supports up to 8.3 names
  const uint8_t chipSelect = 8;

  bool alreadyBegan = false;  // SD.begin() misbehaves if not first call



void setup() 
{
//    Serial.begin(9600);
    Wire.begin();        // Join i2c bus
    
    file = SD.open(fileName);
    SD.begin(chipSelect);
    altimeter.begin(); // Get sensor online
    altimeter.setModeAltimeter(); // Measure pressure in Pascals from 20 to 110 kPa
    altimeter.setOversampleRate(7); // Set Oversample to the recommended 128
    altimeter.enableEventFlags(); // Enable all three pressure and temp event flags 
    //altimeter.setModeActive(); //starts taking measurements!
    
    //If the data file doesn't already exist, just make sure to create it.
    file = SD.open(fileName, FILE_WRITE);
    file.close();
}

void loop(){
  

  file = SD.open(fileName, FILE_WRITE);
  //constantly declaring the values for Accelerometer
    int rawX = analogRead(A0);
    int rawY = analogRead(A1);
    int rawZ = analogRead(A2); 
    float scaledX, scaledY, scaledZ;
     
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
      file.print(F("X: ")); file.println(rawX);
      file.print(F("Y: ")); file.println(rawY);
      file.print(F("Z: ")); file.println(rawZ);
      file.println();
  
  // Print out scaled X,Y,Z accelerometer readings
//      Serial.print(F("X: ")); Serial.print(scaledX); Serial.println(F(" g"));
//      Serial.print(F("Y: ")); Serial.print(scaledY); Serial.println(F(" g"));
//      Serial.print(F("Z: ")); Serial.print(scaledZ); Serial.println(F(" g"));
//      Serial.println();
//      delay(500);
      
      file.print(F("X: ")); file.print(scaledX); file.println(F(" g"));
      file.print(F("Y: ")); file.print(scaledY); file.println(F(" g"));
      file.print(F("Z: ")); file.print(scaledZ); file.println(F(" g"));
      file.println();
      delay(500);
        
  //Altimeter
      pressure = altimeter.readPressure();
      ipress = pressure;

      sprintf(pastring, "%3d", ipress);
    
     
      file.println();

      altitude = altimeter.readAltitude(); //returns a float with meters above sea level. Ex: 1638.94
      alt = altitude;

      previousAltitude = currentAltitude;
      currentAltitude = altitude;

//      Serial.print(F("Current: "));Serial.println(currentAltitude);
//      Serial.print(F("Previous: "));Serial.println(previousAltitude);

      if(currentAltitude + 3 < previousAltitude){
        Serial.print("Deploy"); 
      }
      
      sprintf(pastring, "%3d", alt);
          
      file.print(F("Altitude: "));
      file.print(alt);

      file.println();

      file.print(F("Pressure(Pa): "));
      file.print(pastring);
    
      file.println();
        
      temperature = altimeter.readTempF();
      itemp = temperature;
      sprintf(tmpstring, "%3d", itemp);  

      file.print(F(" Temp(f): "));
      file.print(temperature, 2);
    

      file.println();
      file.close();
      delay(500);
          }

    float mapf(float x, float in_min, float in_max, float out_min, float out_max)
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }


