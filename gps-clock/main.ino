// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_GPS.h"

#include <math.h>

#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);

int lastSecond = 0;
bool ledState = false;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
    // electron asset tracker shield needs this to enable the power to the gps module.
    pinMode(D6, OUTPUT);
    digitalWrite(D6, LOW);

    pinMode(D7, OUTPUT);
    digitalWrite(D7, LOW);

    GPS.begin(9600);
    mySerial.begin(9600);
    Serial.begin(9600);


    //# request a HOT RESTART, in case we were in standby mode before.
    GPS.sendCommand("$PMTK101*32");
    delay(250);


    // request everything!
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);

    // request absolutely everything!
    //GPS.sendCommand("$PMTK314,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1*29");
    delay(250);

    // turn off antenna updates
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(250);
}



void loop() {

    bool hasGPSTime = ((GPS.year != 80) && (GPS.year != 0));

    // process and dump everything from the module through the library.
    while (mySerial.available()) {
        char c = GPS.read();

        if (!hasGPSTime) {
            Serial.print(c);
        }


        if (GPS.newNMEAreceived()) {
            GPS.parse(GPS.lastNMEA());
        }
    }

    // wait 60 seconds, or until we have a fix before we get online to help get a quicker fix.
    if (Particle.connected() == false) {
        if ((GPS.latitude != 0) || (millis() > 120000)) {
            Particle.connect();
        }
    }


    if (GPS.seconds != lastSecond) {
        lastSecond = GPS.seconds;

        // every second, toggle the LED.
        digitalWrite(D7, (ledState) ? HIGH : LOW);
        ledState = !ledState;


        if (hasGPSTime) {
            String currentTime = String::format("GPS TIME is %d/%d/%d at %d:%d:%d, location is %f, %f",
                GPS.month, GPS.day, GPS.year,
                GPS.hour, GPS.minute, GPS.seconds,

                GPS.latitude, GPS.longitude
                );

            Serial.println(currentTime);


        }
        else {
            Serial.println("GPS TIME: searching the skies...");
        }
    }
}
 
 

 
 
//
//void buildGpsDebugOutput() {
//

//    Serial.print("Fix: "); Serial.print((int)GPS.fix);
//    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
//
//    //if (GPS.fix)
//    //{
//    Serial.print("Location: ");
//    Serial.print(GPS.latitude, 4);
//    Serial.print(GPS.lat);
//    Serial.print(", ");
//
//    Serial.print(GPS.longitude, 4);
//    Serial.println(GPS.lon);
//
//    Serial.print("Speed (knots): "); Serial.println(GPS.speed);
//    Serial.print("Angle: "); Serial.println(GPS.angle);
//    Serial.print("Altitude: "); Serial.println(GPS.altitude);
//    Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
//
//    //TODO: BETTER +/- for longitude
//
//   // _gpsDebugLines = ...
//}
//


//int performGPSCommand(String gpsCmd) {
//    nextGpsCommand = gpsCmd;
//
//    String cmd = String::format("$%s*%02x", gpsCmd.c_str(), crc8(gpsCmd));
//    mySerial.println(cmd);
//    return 1;
//}



int crc8(String str) {
  int len = str.length();
  const char * buffer = str.c_str();

  int crc = 0;
  for(int i=0;i<len;i++) {
    crc ^= (buffer[i] & 0xff);
  }
  return crc;
}
