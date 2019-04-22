#include <SparkFunMPL3115A2.h>
#include <Wire.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "SD.h"



  #define Serial_PIN_CS 10


  //altimeter  
  MPL3115A2 altimeter;
  float pressure;
  float temperature;
  float altitude;
  int ipress;
  int itemp;
  int temp;
  int alt;

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
    altimeter.setModeAltimeter(); // Measure pressure in Pascals from 20 to 110 kPa
    altimeter.setOversampleRate(7); // Set Oversample to the recommended 128
    altimeter.enableEventFlags(); // Enable all three pressure and temp event flags 
//    altimeter.setModeActive(); //starts taking measurements!

         
  
//    pinMode(Serial_PIN_CS, OUTPUT);
    // Check if the card is present and can be initialized
//    if (!SD.begin()) {
//        Serial.println(F("Card failed, or not present"));
//        while(1);
//    }
//    Serial.println(F("card initialized."));
    
}

void loop(){
  //constantly declaring the values for Accelerometer
    int rawX = analogRead(A0);
    int rawY = analogRead(A1);
    int rawZ = analogRead(A2); 
    float scaledX, scaledY, scaledZ;
     
  
  
   sdFile = SD.open("datalog.txt", FILE_WRITE);

//    if (sdFile) {
        
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
      Serial.print(F("X: ")); Serial.println(rawX);
      Serial.print(F("Y: ")); Serial.println(rawY);
      Serial.print(F("Z: ")); Serial.println(rawZ);
      Serial.println();
  
  // Print out scaled X,Y,Z accelerometer readings
      Serial.print(F("X: ")); Serial.print(scaledX); Serial.println(F(" g"));
      Serial.print(F("Y: ")); Serial.print(scaledY); Serial.println(F(" g"));
      Serial.print(F("Z: ")); Serial.print(scaledZ); Serial.println(F(" g"));
      Serial.println();
  
      delay(500);
        
  //Altimeter
      pressure = altimeter.readPressure();
      ipress = pressure;

      sprintf(pastring, "%3d", ipress);
    
     
      Serial.println();

      altitude = altimeter.readAltitude(); //returns a float with meters above sea level. Ex: 1638.94
      alt = altitude;

      
      sprintf(pastring, "%3d", alt);

       Serial.print(F("Altitude: "));
       Serial.print(alt);

     Serial.println();

      Serial.print(F("Pressure(Pa): "));
      Serial.print(pastring);
    
      Serial.println();
        
      temperature = altimeter.readTempF();
      itemp = temperature;
      sprintf(tmpstring, "%3d", itemp);  

      Serial.print(F(" Temp(f): "));
      Serial.print(temperature, 2);
    

      Serial.println();

      delay(500);
      
//    }else{
        // if the file didn't open, print an error
        Serial.println(F("error opening file."));   
//    }
}

    float mapf(float x, float in_min, float in_max, float out_min, float out_max)
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }



 

    



