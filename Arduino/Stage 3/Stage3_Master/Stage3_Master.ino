/*
      STAGE #2 - Arduino Nano

      Pump Water:
        Disable all -> w 0.0 0
        Enable 1    -> w 0.5 1
        Enable 2    -> w 0.5 2
        Enable 3    -> w 0.5 3
        Enable 4    -> w 0.5 4

      Pump PEG:
        Disable all -> p 0.0 0
        Enable 1    -> p 0.5 1
        Enable 2    -> p 0.5 2
        Enable 3    -> p 0.5 3
        Enable 4    -> p 0.5 4

      Mist:
        Disable all -> m 0.0 0
        Enable all  -> m 0.0 1

*/
#include <stdio.h>
#include <string.h>

#include <Wire.h>
#define BUFFER_SIZE 4
#define READ_CYCLE_DELAY 1000

// Slave with Water Pumps = 8
// Slave with Peg Pumps = 9
int slaveAddress[] = {8, 9};

byte buffer[BUFFER_SIZE];

int command = 0;

byte mistPins[] = {4, 5};
int mistActiveSeconds = 5;

void setup() {
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.begin(9600);

  InitializeMistMakers();
}

void InitializeMistMakers()
{
  for (int i = 0; i < sizeof(mistPins); i++)
  {
    pinMode(mistPins[i], OUTPUT);
  }
}

/*
      MIST VARIABLES
*/
bool isMakingMist = false;
unsigned long mistTimer = 0;
float mistTime;

void DOMist(bool value)
{
  for (int i = 0; i < sizeof(mistPins); i++)
  {
    digitalWrite(mistPins[i], value ? HIGH : LOW);
  }
}

void HandleMistMakers()
{
  if (isMakingMist)
  {
    if (millis() - mistTimer >= mistTime)
    {
      DOMist(false);
      isMakingMist = false;
      mistTimer = millis();
    }
  }
}

void loop() {
  // Handle Serial
  HandleSerialCOM();
  HandleMistMakers();
}

String msg;
char msgType;
int msgValue;
float msgMagnitude;

const char water = 'w';
const char peg = 'p';
const char mist = 'm';

const char delimiter = ' ';

void HandleSerialCOM()
{
  if (Serial.available() > 0) {
    msg = Serial.readStringUntil('\n');

    if (msg.length() > 0)
    {
      msgType = msg.substring(0, 1)[0];

      if (msgType == water)
      {
        msgMagnitude = (float)msg.substring(2, msg.length() - 2).toFloat();
        msgValue = msg.substring(msg.length() - 2).toInt();

        // Send Command
        SendCommand(slaveAddress[0], msgValue + 1, msgMagnitude);
      }

      if (msgType == peg)
      {
        msgMagnitude = msg.substring(2, msg.length() - 2).toFloat();
        msgValue = msg.substring(msg.length() - 2).toInt();

        // Send Command
        SendCommand(slaveAddress[1], msgValue + 1, msgMagnitude);
      }

      if (msgType == mist)
      {
        msgMagnitude = msg.substring(2).toFloat();
        mistTime = msgMagnitude;
        mistTimer = millis();

        DOMist(msgValue);
        isMakingMist = true;
      }
    }
  }
}

/*
    HANDLE I2C COMMUNICATION
      - Send Command to target pump
*/
short data[BUFFER_SIZE];

void SendCommand(int _slaveAddress, int targetPump, int _time)
{

  data[0] = targetPump;
  data[1] = _time;

  byte myArray[2];
  myArray[0] = (_time >> 8) & 0xFF;
  myArray[1] = _time & 0xFF;
  Wire.beginTransmission(_slaveAddress);
  Wire.write(myArray, 2);
  Wire.endTransmission();
  delay(50);
  Serial.println(data[0]);
  Wire.beginTransmission(_slaveAddress);    // Transmit to slaveAddress
  Wire.write(targetPump);
  Wire.endTransmission();
}
