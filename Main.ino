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
  float initialHeight;
  float failCheck;
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
  char fileName[] = "data.txt";
  const uint8_t chipSelect = 8;
  
  //Explosive Charge
    int explosiveCharge = 7; //this is the pin number

void setup() 
{
//  Serial.begin(9600);
    Wire.begin();        // Join i2c bus
    
    file = SD.open(fileName);
    SD.begin(chipSelect);
    
    altimeter.begin(); // Get sensor online
    altimeter.setModeAltimeter(); // Measure pressure in Pascals from 20 to 110 kPa
    altimeter.setOversampleRate(7); // Set Oversample to the recommended 128
    altimeter.enableEventFlags(); // Enable all three pressure and temp event flags 
    
    //If the data file doesn't already exist, just make sure to create it.
    file = SD.open(fileName, FILE_WRITE);
    

    initialHeight = altimeter.readAltitude();
    pinMode(explosiveCharge, OUTPUT);
    digitalWrite(explosiveCharge, LOW); //Turns off voltage from that pin  

//   Serial.println("Initial Height: ");Serial.print(initialHeight);
     file.println(F("rawX,rawY,rawZ,scaledX,scaledY,scaledZ, currentAltitude, preassure, temperature,failCheck"));
     file.println();
     file.println(F("Initial Height: "));file.print(initialHeight);
     file.println(); file.println(); 
      
     file.close();
     
}
void loop(){
  

  file = SD.open(fileName, FILE_WRITE);
  //constantly declaring the values for Accelerometer
    int rawX = analogRead(A2);
    int rawY = analogRead(A1);
    int rawZ = analogRead(A0); 
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
      file.print(rawX); file.print(",");
      file.print(rawY); file.print(",");
      file.print(rawZ); file.print(",");
  
  // Print out scaled X,Y,Z accelerometer readings

      
      file.print(scaledX); file.print(F(","));
      file.print(scaledY); file.print(F(","));
      file.print(scaledZ); file.print(F(","));
        
  //Altimeter
      pressure = altimeter.readPressure();
      ipress = pressure;

      sprintf(pastring, "%3d", ipress);
    
      altitude = altimeter.readAltitude(); 
      alt = altitude;

      previousAltitude = currentAltitude;
      currentAltitude = altitude;
      failCheck = currentAltitude - initialHeight;
      

//      Serial.print(F("Current: "));Serial.println(currentAltitude);
//   Serial.print(F("Previous: "));Serial.println(previousAltitude);
//     Serial.print(F("FailCheck: ")); Serial.print(failCheck);
//     Serial.println();
      
      sprintf(pastring, "%3d", alt);
          
      
      file.print(currentAltitude); file.print(F(","));

      file.print(pastring); file.print(F(","));
    
      temperature = altimeter.readTempF();
      itemp = temperature;
      sprintf(tmpstring, "%3d", itemp);  

     
      file.print(temperature, 2); file.print(F(","));
    
       if(currentAltitude + .8 < previousAltitude && failCheck > 70 ){  //There is a +1 so the parachute won't deploy because of noise

//        if(currentAltitude != previousAltitude){
//        Serial.print("DEPLOY");
        file.println("Deploy");
        digitalWrite(explosiveCharge, HIGH);  //Turns on voltage for thar pin
      }


      

      file.print(failCheck);
      file.println();
      file.println();
      
      file.close();
      
      delay(200);
      
          }

    float mapf(float x, float in_min, float in_max, float out_min, float out_max)
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }


