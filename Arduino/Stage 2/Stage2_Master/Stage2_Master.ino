/*
      STAGE #2 - Arduino Nano

      Pump Water:
        Disable all -> w0
        Enable 1    -> w1
        Enable 2    -> w2
        Enable 3    -> w3
        Enable 4    -> w4

      Pump PEG:
        Disable all -> p0
        Enable 1    -> p1
        Enable 2    -> p2
        Enable 3    -> p3
        Enable 4    -> p4

      Mist:
        Disable all -> m0
        Enable all  -> m1

*/


#include <Wire.h>
#define BUFFER_SIZE  8
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

void DOMist(bool value)
{
  for (int i = 0; i < sizeof(mistPins); i++)
  {
    digitalWrite(mistPins[i], value ? HIGH : LOW);
  }
}

void loop() {
  // Handle Serial
  HandleSerialCOM();
}

String msg;
char msgType;
int msgValue;

const char water = 'w';
const char peg = 'p';
const char mist = 'm';

void HandleSerialCOM()
{
  if (Serial.available() > 0) {
    msg = Serial.readStringUntil('\n');

    if (msg.length() > 0)
    {
      msgType = msg.substring(0, 1)[0];
      msgValue = msg.substring(1, msg.length()).toInt();

      if (msgType == water)
      {
        // Send Command
        SendCommand(slaveAddress[0], msgValue);
      }

      if (msgType == peg)
      {
        // Send Command
        SendCommand(slaveAddress[1], msgValue);
      }

      if (msgType == mist)
      {
        // DOMist
        DOMist(msgValue);
//        delay(mistActiveSeconds * 1000);
//        DOMist(false);
      }
    }
  }
}

/*
    HANDLE I2C COMMUNICATION
      - Send Command to pump water
*/
void SendCommand(int _slaveAddress, int targetPomp)
{
  Wire.beginTransmission(_slaveAddress);    // Transmit to slaveAddress
  Wire.write(targetPomp);
  Wire.endTransmission();                   // Stop transmitting
}
