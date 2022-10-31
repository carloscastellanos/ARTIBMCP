/*
      STAGE #1 - Arduino Mega

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

      GrowLights:
        Disable all -> l 0
        Enable all  -> l 1

      ------------------------ Retreive Info

      Full Message Example: m 55.8 t 21.4 l 1
      Temperature:
        Get Value -> t 0.0

      Humidity:
        Get Value -> h 0.0

      GrowLights:
        Status ON  -> l 1
        Status OFF -> l 0

*/

#include <stdio.h>
#include <string.h>
#include <Wire.h>

/*
      CORE VARIABLES DEFINITION
*/

// Slave with Water Pumps = 8
// Slave with Peg Pumps = 9
const int slaveAddress[] = {8, 9};

const byte mistPins[] = {8, 9, 10, 11};
#define THERMISTORPIN A12
const byte growLights_PIN[] = {12, 13};
const byte heatingPads_PIN[] = {4, 5};

// Commands to trigger actions
const char water = 'w';
const char peg = 'p';
const char mist = 'm';
const char heatingPads = 't';  // ?????

const char humidity = 'h';
const char temperature = 't';
const char growLights = 'l';

const char delimiter = ' ';

void setup() {
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.begin(9600);

  InitializeMistMakers();
  InitializeHumidityHandler();
  InitializeGrowLights();
  InitializeHeatingPads();
}

void loop() {
  HandleSerialCOM();
  HandleMistMakers();
  HandleHumidity();
  HandleTemperature();
  HandleGrowLights();
  HandleHeatingPads();
}

/*
      MIST MAKERS
*/
bool isMakingMist = false;
unsigned long mistTimer = 0;
float mistTime;

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

/*
      HUMIDITY SENSOR
*/

float humidityTimer;
int humidityfrequency = 5000;
float humidity_values[4] = {0.3, 0.43, 0.5, 0};
float humidity_mean = 0;

void InitializeHumidityHandler()
{
  humidityTimer = millis();
}

void HandleHumidity()
{
  if (millis() - humidityTimer >= humidityfrequency)
  {
    humidityTimer = millis();
    //RequestInfo(slaveAddress[1]);
    humidity_mean = humidity_values[0] + humidity_values[1] + humidity_values[2] + humidity_values[3];
    humidity_mean = humidity_mean / sizeof(humidity_values);
  }
}

/*
      THERMISTOR
      Source: https://learn.adafruit.com/thermistor/using-a-thermistor
*/

// the value of the 'other' resistor
#define SERIESRESISTOR 10000

float thermistorValue;

void HandleTemperature()
{
  thermistorValue = analogRead(THERMISTORPIN);
  thermistorValue = (1023 / thermistorValue) - 1;
  thermistorValue = SERIESRESISTOR / thermistorValue;
}

/*
      GROW LIGHTS
*/
bool growLights_state = false;

void InitializeGrowLights()
{
  for (int i = 0; i < sizeof(growLights_PIN); i++)
  {
    pinMode(growLights_PIN[i], OUTPUT);
  }
}

void HandleGrowLights()
{
  for (int i = 0; i < sizeof(growLights_PIN); i++)
  {
    digitalWrite(growLights_PIN[i], growLights_state ? LOW : HIGH);
  }
}

/*
      HEATING PADS
*/
bool heatingPads_state = false;

void InitializeHeatingPads()
{
  for (int i = 0; i < sizeof(heatingPads_PIN); i++)
  {
    pinMode(heatingPads_PIN[i], OUTPUT);
  }
}

void HandleHeatingPads()
{
  for (int i = 0; i < sizeof(heatingPads_PIN); i++)
  {
    digitalWrite(heatingPads_PIN[i], heatingPads_state ? HIGH : LOW);
  }
}


/*
    HANDLE I2C COMMUNICATION
      - Send Command to target pump
      - Retrieve Humidity Information
*/
#define BUFFER_SIZE 8
byte buffer[BUFFER_SIZE];

String msg;
char msgType;
int msgValue;
float msgMagnitude;

String info2retrieve;
char value2retrieve[3];

void HandleSerialCOM()
{
  if (Serial.available() > 0) {
    msg = Serial.readStringUntil('\n');

    if (msg.length() > 0)
    {
      msgType = msg.substring(0, 1)[0];

      if (msgType == water)
      {
        msgMagnitude = msg.substring(2, msg.length() - 2).toFloat();
        msgValue = msg.substring(msg.length() - 2).toInt();

        SendCommand(slaveAddress[0], msgValue + 1, msgMagnitude);
      }

      if (msgType == peg)
      {
        msgMagnitude = msg.substring(2, msg.length() - 2).toFloat();
        msgValue = msg.substring(msg.length() - 2).toInt();

        SendCommand(slaveAddress[1], msgValue + 1, msgMagnitude);
      }

      if (msgType == mist)
      {
        msgMagnitude = msg.substring(2).toFloat();
        mistTime = msgMagnitude;
        mistTimer = millis();

        DOMist(true);
        isMakingMist = true;
      }

      if (msgType == growLights)
      {
        msgValue = msg.substring(msg.length() - 2).toInt();
        growLights_state = msgValue;
      }

      if (msgType == heatingPads)
      {
        msgValue = msg.substring(msg.length() - 2).toInt();
        heatingPads_state = msgValue;
      }

      Serial.print(humidity);
      Serial.print(delimiter);
      Serial.print(humidity_mean, 2); // Second parameter defines the decimal places to use
      Serial.print(temperature);
      Serial.print(delimiter);
      Serial.print(thermistorValue, 2); // Second parameter defines the decimal places to use
      Serial.print(growLights);
      Serial.print(delimiter);
      Serial.print((int)growLights_state);
      Serial.println();
    }
  }
}

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
    Serial.print("Slave address ");
    Serial.print(slaveAdd);
    Serial.print(": moisture1 = ");
    Serial.print(moisture1_value);
    Serial.print(": moisture2 = ");
    Serial.print(moisture3_value);
    Serial.print(": moisture3 = ");
    Serial.print(moisture3_value);
    Serial.print(": moisture4 = ");
    Serial.println(moisture4_value);
  }
}
