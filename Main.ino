/**
 * Welcome to my flight computer code!
 * I am going to assume that you know very little about coding
 * If you arn't new sorry for all the comments!
 * - Brandon Comins 
 * 2019
 */

// Before you push code make sure to go to Tools > Board to select your board
// Before you push code make sure to go to Tools > Port to select your port (will be different for every Arduino).

/** 
 * These are libraries.
 * To add one go to sketch > include Library 
 * If it is a custom library download the custom 
 * library and put it in a library folder with the code 
 * in the main directoy then go to sketch > include Library > Add .ZIP library
 */
#include <SD.h>
#include <SPI.h>
#include <SparkFunMPL3115A2.h>
#include <Wire.h>
#include "Arduino.h"
#include "SoftwareSerial.h"


  //Variables for the altimeter  
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

  //Variables for the accelerometer
  int scale = 3;
  boolean micro_is_5V = true;

  //Variables for the SD card reader
  File file;
  char fileName[] = "data.txt";
  const uint8_t chipSelect = 8; //pin number in arduino
  
  //Variable for the explosive charge
    int explosiveCharge = 7; //this is the pin number

void setup() 
/**
 * The setup function only runs once when the arduino is turned on
 */
{
  /**
   * Use the serial monitor to debug things.
   * This gives you live data, but your arduino 
   * has to be plugged in or connected via bluetooth
   */
//  Serial.begin(9600); 
    Wire.begin();        // Join i2c bus
    
    file = SD.open(fileName); //opens file on SD card
    
    SD.begin(chipSelect); // Begins SD caed
    
    altimeter.begin(); // Get sensor online
    altimeter.setModeAltimeter(); // Measure pressure in Pascals from 20 to 110 kPa
    altimeter.setOversampleRate(7); // Set Oversample to the recommended 128
    altimeter.enableEventFlags(); // Enable all three pressure and temp event flags 
    
    //If the data file doesn't already exist, just make sure to create it.
    file = SD.open(fileName, FILE_WRITE);
    

    initialHeight = altimeter.readAltitude(); //Giving a value to intial height
    
    pinMode(explosiveCharge, OUTPUT); // Tells the pin to be ready to output voltage
    digitalWrite(explosiveCharge, LOW); //Turns off voltage from that pin  

//   Serial.println("Initial Height: ");Serial.print(initialHeight); //print to the serial monitor for debugging

    /**
     * (filename).println writes to the file on the SD card.
     * the capital F is there to reduce ram usage.
     * This only works when you are only printing a string.
     * The format that I print to the SD card creates a legend 
     * and allows the data to be copy pasted easily to microsoft Excel, and be seperated by colums.
     */
     file.println(F("rawX,rawY,rawZ,scaledX,scaledY,scaledZ, currentAltitude, preassure, temperature,failCheck"));
     file.println();
     file.println(F("Initial Height: "));file.print(initialHeight);
     file.println(); file.println(); 
      
     file.close(); // It is neccesary to close the file after writing to it.
     
}
void loop(){
  /**
   * Loop function runs forever.
   * Once it finish all the code in the loop, 
   * it "loops" back to the begging and runs the code again
   */

  file = SD.open(fileName, FILE_WRITE); //Re-opens the file
  //constantly declaring the values for accelerometer
    int rawX = analogRead(A2);
    int rawY = analogRead(A1);
    int rawZ = analogRead(A0); 
    //Creating variables for scaled accelerometer values
    float scaledX, scaledY, scaledZ;

   /** This changes math on scaled data for the accelerometer based off of if you wired it to 5v or 3.3v. 
    *  If you read the documentation of the sensor you will see why this is here
    */
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
      
      // Used for debugging
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

      file.print(failCheck);
      file.println();
      file.println();

    /** This if statement deploys the parachute if the failcheck is greater than 70 meters 
     *  and current altitude < previous altitude.
     *  The failcheck will prevent the explosive charge to blow up if it does not reach 70 meters (or 200ish feet).
     *  There is a +.8 so the parachute won't deploy because of noise
     */
       if(currentAltitude + .8 < previousAltitude && failCheck > 70 ){  
       //These two commented lines are used for debugging. If you need to test if voltage is being sent (by using a multimeter).
       //if (currentAltitude != previousAltitude){  
       //Serial.print("DEPLOY");
        file.println("Deploy");
        digitalWrite(explosiveCharge, HIGH);  //Turns on voltage for that pin to explode the explosive charge
        file.println();
        file.println();
      }

 
      
      file.close();

      //delays the loop function for 200 miliseconds.
      delay(200);
      
          }
    //function to handle math for scaled acceleration data
    float mapf(float x, float in_min, float in_max, float out_min, float out_max)
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }


