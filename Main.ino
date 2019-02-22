#include "Arduino.h"
#include "I2Cdev.h"
#include "ADXL345.h"
#include "SFE_BMP180.h"
#include "SoftwareSerial.h"
#include "Adafruit_GPS.h"
#include "SD.h"



// Pin Definitions
#define GPSADAFUIT_PIN_RX  3
#define GPSADAFUIT_PIN_TX 2
#define SDFILE_PIN_CS 10



// Global variables and defines

int16_t adxlAx, adxlAy, adxlAz;
#define GPSECHO  true
uint32_t timer = millis(); 
SoftwareSerial gpsSerial(GPSADAFUIT_PIN_TX,GPSADAFUIT_PIN_RX);
Adafruit_GPS gpsAdafuit(&gpsSerial);
File sdFile;
ADXL345 adxl;
SFE_BMP180 bmp180;



const int timeout = 10000;       //define timeout of 10 sec
long time0;

void setup() 
{
    Serial.begin(9600);
    
    adxl.init();

 bmp180.begin();
    gpsAdafuit.init();
  
    pinMode(SDFILE_PIN_CS, OUTPUT);
    // Check if the card is present and can be initialized
    if (!SD.begin()) {
        Serial.println(F("Card failed, or not present"));
        while(1);
    }
    Serial.println(F("card initialized."));
    
}

void loop()
{
   sdFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file exists in SD card, write sensor data
    if (sdFile) {
      writeData();
    }else{
        // if the file didn't open, print an error
        Serial.println(F("error opening file."));   
    }
      if (timer > millis())  timer = millis();
}

    void writeData(){
         
    //Accelerometer
      adxl.getAcceleration(&adxlAx, &adxlAy, &adxlAz);
      
      sdFile.println(F("Accelerometer Data")); 
      sdFile.println(("Accelerometer X: " + adxlAx)); 
      sdFile.println(("Accelerometer Y: " + adxlAy));   
      sdFile.println(("Acceleromete Z: " + adxlAz));

    //Altimeter
      double bmp180Alt = bmp180.altitude();
      double bmp180Pressure = bmp180.getPressure();
      double bmp180TempC = bmp180.getTemperatureC();

        
      sdFile.print(bmp180Alt,1); sdFile.print(F(" [m]"));
      sdFile.print(bmp180Pressure,1); sdFile.print(F(" [hPa]"));
      sdFile.print(bmp180TempC,1); sdFile.println(F(" [°C]"));

      //GPS
    
      gpsSerial.println(F("Altimeter Data"));
      gpsSerial.println(F("Altitude: "));
      gpsSerial.println(F("\tpressure: ")); 
      gpsSerial.println(F("\tTemperature: ")); 


      if (millis() - timer > 2000) {
        timer = millis(); // reset the timer

        gpsSerial.println(F("\nTime: "));
        gpsSerial.println(gpsAdafuit.hour, DEC); gpsSerial.print(':');
        gpsSerial.println(gpsAdafuit.minute, DEC); gpsSerial.print(':');
        gpsSerial.println(gpsAdafuit.seconds, DEC); gpsSerial.print('.');
        gpsSerial.println(gpsAdafuit.milliseconds);
        gpsSerial.println(F(" quality: ")); gpsSerial.println((int)gpsAdafuit.fixquality);
        if (gpsAdafuit.fix) {
          gpsSerial.println(F("Location: "));
          gpsSerial.println(gpsAdafuit.latitude, 4); gpsSerial.print(gpsAdafuit.lat);
          gpsSerial.println(gpsAdafuit.longitude, 4); gpsSerial.println(gpsAdafuit.lon);
          gpsSerial.println("Location (in degrees, works with Google Maps): ");
          gpsSerial.println(gpsAdafuit.latitudeDegrees, 4);
          gpsSerial.println(gpsAdafuit.longitudeDegrees, 4);

          gpsSerial.println("Speed (knots): "); gpsSerial.println(gpsAdafuit.speed);
          gpsSerial.println("Angle: "); gpsSerial.println(gpsAdafuit.angle);
          gpsSerial.println("Altitude: "); gpsSerial.println(gpsAdafuit.altitude);
          gpsSerial.println("Satellites: "); gpsSerial.println((int)gpsAdafuit.satellites);
        }
      }
    }


 

    



