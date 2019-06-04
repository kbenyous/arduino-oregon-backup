/**
 *******************************
 *
 * Version 1.0 - Hubert Mickael <mickael@winlux.fr> (https://github.com/Mickaelh51)
 *  - Clean ino code
 *  - Add MY_DEBUG mode in library
 * Version 0.2 (Beta 2) - Hubert Mickael <mickael@winlux.fr> (https://github.com/Mickaelh51)
 *  - Auto detect Oregon 433Mhz
 *  - Add battery level
 *  - etc ...
 * Version 0.1 (Beta 1) - Hubert Mickael <mickael@winlux.fr> (https://github.com/Mickaelh51)
 *
 *******************************
 * DESCRIPTION
 * This sketch provides an example how to implement a humidity/temperature from Oregon sensor.
 * - Oregon sensor's battery level
 * - Oregon sensor's id
 * - Oregon sensor's type
 * - Oregon sensor's channel
 * - Oregon sensor's temperature
 * - Oregon sensor's humidity
 *
 * Arduino UNO <-- (PIN 2) --> 433Mhz receiver <=============> Oregon sensors
 */

// Enable debug prints
#define MY_DEBUG

#include <SPI.h>
#include <EEPROM.h>
#include "OregonV2Decoder.h"




//Define pin where is 433Mhz receiver (here, pin 2)
#define MHZ_RECEIVER_PIN 2
//Define maximum Oregon sensors (here, 3 differents sensors)
#define COUNT_OREGON_SENSORS 3

#define PIN_INPUT_LED 13


OregonV2Decoder orscV2 = OregonV2Decoder();

volatile word pulse;

void setup ()
{
Serial.begin(115200);
  Serial.println("Setup started");
 
  pinMode(MHZ_RECEIVER_PIN, INPUT);
  //Setup received data
  attachInterrupt(digitalPinToInterrupt(MHZ_RECEIVER_PIN), ext_int_1, CHANGE);

  pinMode(PIN_INPUT_LED, OUTPUT);
  

  Serial.println("Setup completed");

  
}


void loop () {
    //------------------------------------------
    //Start process new data from Oregon sensors
    //------------------------------------------
    cli();
    word p = pulse;
    pulse = 0;
    sei();
    if (p != 0)
    {
        if (orscV2.nextPulse(p))
        {
            digitalWrite(PIN_INPUT_LED,HIGH);
            //Decode Hex Data once
            const byte* DataDecoded = DataToDecoder(orscV2);
            //Find or save Oregon sensors's ID
            //int SensorID = FindSensor(id(DataDecoded),COUNT_OREGON_SENSORS);

            // just for DEBUG
            OregonType(DataDecoded);
            channel(DataDecoded);
            temperature(DataDecoded);
            humidity(DataDecoded);
            battery(DataDecoded);
            digitalWrite(PIN_INPUT_LED,LOW);
        }

    }
}



void ext_int_1(void)
{
    static word last;
    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;
}

float temperature(const byte* data)
{
  int sign = (data[6]&0x8) ? -1 : 1;
  float temp = ((data[5]&0xF0) >> 4)*10 + (data[5]&0xF) + (float)(((data[4]&0xF0) >> 4) / 10.0);
  float result = sign * temp;
  #ifdef MY_DEBUG
    Serial.println("Oregon temperature: " + String(result));
  #endif
  return sign * temp;
}


byte humidity(const byte* data)
{
  byte humidity = (data[7]&0xF) * 10 + ((data[6]&0xF0) >> 4);
  #ifdef MY_DEBUG
    Serial.println("Oregon humidity: " + String(humidity));
  #endif
  return (humidity);
}

//10 => battery lovel is LOW
//90 => battery level is HIGH
byte battery(const byte* data)
{
  byte BatteryLevel = (data[4] & 0x4) ? 10 : 90;
  #ifdef MY_DEBUG
    Serial.println("Oregon battery level: " + String(BatteryLevel));
  #endif
  return BatteryLevel;
}

//Find id from Oregon Sensors
byte id(const byte* data)
{
  #ifdef MY_DEBUG
    Serial.print("Oregon ID: " + String(data[3]) + " Hexadecimal: ");
    Serial.println(data[3],HEX);
  #endif
  return (data[3]);
}

//Find Channel from Oregon Sensors
byte channel(const byte* data)
{
    byte channel;
    switch (data[2])
    {
        case 0x10:
            channel = 1;
            break;
        case 0x20:
            channel = 2;
            break;
        case 0x40:
            channel = 3;
            break;
        default:
            channel = 0;
            break;
     }
     #ifdef MY_DEBUG
       Serial.println("Oregon channel: " + String(channel));
     #endif
     return channel;
}

// Detect type of sensor module
const char* OregonType (const byte* data)
{
    const char* Model;
    if(data[0] == 0xEA && data[1] == 0x4C)
    {
        Model = "THN132N";
    }
    else if(data[0] == 0x1A && data[1] == 0x2D)
    {
	       Model = "THGR228N";
    }
    else
    {
	      Model = "UNKNOWN";
    }
    #ifdef MY_DEBUG
      Serial.println("Oregon model: " + String(Model));
    #endif

    return Model;
}

// Decode data once
const byte* DataToDecoder (class OOKDecoder& decoder)
{
    byte pos;
    const byte* data = decoder.getData(pos);
    #ifdef MY_DEBUG
      Serial.println("Brute Hexadecimal data from sensor: ");
      for (byte i = 0; i < pos; ++i) {
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0x0F, HEX);
      }
      Serial.println(' ');
    #endif
    decoder.resetDecoder();
    return data;
}

