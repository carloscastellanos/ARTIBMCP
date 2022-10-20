#include <Wire.h>
int x = 0;

#define BUFFER_SIZE  8
#define READ_CYCLE_DELAY 1000

int slaveAddress[] = {8};
byte buffer[BUFFER_SIZE];

/*
    command to be sent through I2C to the watering Kit
      0 -> All pomps off
      1 -> First pomp on
      2 -> Second pomp on
      3 -> Third pomp on
      4 -> Fourth pomp on
*/
int command = 0;

void setup() {
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  // Handle Serial
  HandleSerialCOM();

  // delay(READ_CYCLE_DELAY);
}

/*
  SERIAL COMMUNICATION
   PROTOCOL
   a=auxin/aba
   p=peg
   w=water
   l=light
   t=temperature

   a, b, c, d -> motors ID
   0 or 1 ON/OFF

   m -> mist ID
   0 or 1 ON/OFF
*/

String msg;
char msgType;
int msgValue;

const char auxin = 'a';
const char peg = 'p';
const char water = 'w';
const char light = 'l';
const char temperature = 't';

void HandleSerialCOM()
{
  if (Serial.available() > 0) {
    msg = Serial.readStringUntil('\n');

    if (msg.length() > 0)
    {
      msgType = msg.substring(0, 1)[0];
      msgValue = msg.substring(1, msg.length()).toInt();

      if (msgType == 'a')
      {
        // Send Command
        SendCommand(slaveAddress[0], msgValue);

        // Request Information
        RequestInfo(slaveAddress[0]);
      }

      Serial.println("m 75");
      Serial.println("t 23");
      Serial.println("l 0");
    }
  }
}

/*
    HANDLE I2C COMMUNICATION
      - Send Command to pomp water
      - Request Info to read the sensors' data
*/

void SendCommand(int _slaveAddress, int targetPomp)
{
  Wire.beginTransmission(_slaveAddress);    // Transmit to slaveAddress
  Wire.write(targetPomp);
  Wire.endTransmission();                   // Stop transmitting
}

void RequestInfo(int slaveAdd) {
  Wire.requestFrom(slaveAdd, BUFFER_SIZE);    // request data from the slave
  if (Wire.available() == BUFFER_SIZE)
  { // if the available data size is same as I'm expecting
    // Reads the buffer the slave sent
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      buffer[i] = Wire.read();  // gets the data
    }

    // Parse the buffer
    // In order to convert the incoming bytes info short, I use union
    union short_tag {
      byte b[2];
      short val;
    } short_cast;

    // Parse the temperature
    short_cast.b[0] = buffer[0];
    short_cast.b[1] = buffer[1];
    float moisture1_value = ((float)(short_cast.val)) / 10;

    // Parse the moisture
    short_cast.b[0] = buffer[2];
    short_cast.b[1] = buffer[3];
    float moisture2_value = ((float)(short_cast.val)) / 10;

    short_cast.b[0] = buffer[4];
    short_cast.b[1] = buffer[5];
    short moisture3_value = short_cast.val;

    short_cast.b[0] = buffer[6];
    short_cast.b[1] = buffer[7];
    short moisture4_value = short_cast.val;

    // Prints the income data
    //    Serial.print("Slave address ");
    //    Serial.print(slaveAdd);
    //    Serial.print(": moisture1 = ");
    //    Serial.print(moisture1_value);
    //    Serial.print(": moisture2 = ");
    //    Serial.print(moisture3_value);
    //    Serial.print(": moisture3 = ");
    //    Serial.print(moisture3_value);
    //    Serial.print(": moisture4 = ");
    //    Serial.println(moisture4_value);
  }
}
