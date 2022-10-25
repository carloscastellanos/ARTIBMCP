#include <Wire.h>
#include "Wire.h"

// set all moisture sensors PIN ID
int moisture1 = A0;
int moisture2 = A1;
int moisture3 = A2;
int moisture4 = A3;

// declare moisture values
int moisture1_value = 0;
int moisture2_value = 0;
int moisture3_value = 0;
int moisture4_value = 0;


// set water relays
int relay1 = 6;
int relay2 = 8;
int relay3 = 9;
int relay4 = 10;

int relays[] = {relay1, relay2, relay3, relay4};

// set water pump
int pump = 4;

// set button
int button = 12;

//pump state    1:open   0:close
int pump_state_flag = 0;

//relay1 state    1:open   0:close
int relay1_state_flag = 0;

//relay2 state   1:open   0:close
int relay2_state_flag = 0;

//relay3 state  1:open   0:close
int relay3_state_flag = 0;

//relay4 state   1:open   0:close
int relay4_state_flag = 0;

int relaysFlags[] = {relay1_state_flag, relay2_state_flag, relay3_state_flag, relay4_state_flag};

/*
 *                I2C
                       pump &   activeTime
   Expecting to receive int char float
                  bytes: 4  1    4       = 9
                example: 1&1000
 */

#define BUFFER_SIZE 4
short data[BUFFER_SIZE];
int incomingMSG;

byte slaveAddress = 9;

void setup()
{
  Serial.begin(9600);

  // Initialize PEG Pumps
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  // declare pump as output
  pinMode(pump, OUTPUT);

  // Initialize I2C
  Wire.begin(slaveAddress);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void requestEvent()
{
  data[0] = moisture1_value; // In order to use short, I multiple by 10
  data[1] = moisture2_value;
  data[2] = moisture3_value;
  data[3] = moisture4_value;
  Wire.write((byte*)data, BUFFER_SIZE * sizeof(short));
}

void loop()
{
  HandlePumpTimer(1);
  HandlePumpTimer(2);
  HandlePumpTimer(3);
  HandlePumpTimer(4);

  ReadMoisture();
  ReadTemperature();
}

int currentPump;
float activeTime;

void receiveEvent(int bytes) {
  incomingMSG = Wire.read();    // read one character from the I2C
  Serial.print("Something reveiced: ");
  Serial.println(incomingMSG);

  if (incomingMSG == 0)
  {
    Serial.println("Turn OFF All Pomps");
    for (int i = 0; i < sizeof(relaysFlags); i++)
    {
      digitalWrite(relays[i], HIGH);
    }
    for (int i = 0; i < sizeof(relaysFlags); i++)
    {
      digitalWrite(relays[i], LOW);
    }
    delay(1000);
    for (int i = 0; i < sizeof(relaysFlags); i++)
    {
      stopWater(i);
    }
  } else
  {
    currentPump = incomingMSG.substring(0, 1).toInt();
    activeTime = incomingMSG.substring(2).toFloat();
    
    waterFlower(currentPump);
    pumpTime[currentPump - 1] = activeTime;
    pumpTimer[pumpNumber - 1] = millis();
  }
}

void waterFlower(int pumpNumber) {
  digitalWrite(relays[pumpNumber - 1], HIGH);
  relaysFlags[pumpNumber - 1] = 1;
  delay(50);
  if (pump_state_flag == 0)
  {
    digitalWrite(pump, HIGH);
    pump_state_flag = 1;
    delay(50);
  }

  //  delay(1000000);
  //  stopWater(pumpNumber);
}

void stopWater(int pumpNumber) {
  digitalWrite(relays[pumpNumber - 1], LOW);
  relaysFlags[pumpNumber - 1] = 0;
  delay(50);
  if ((relaysFlags[0] == 0) && (relaysFlags[1] == 0) && (relaysFlags[2] == 0) && (relaysFlags[3] == 0))
  {
    digitalWrite(pump, LOW);
    pump_state_flag = 0;
    delay(50);
  }
}

unsigned long pumpTimer[4] = {0, 0, 0, 0};
float pumpTime[4] = {0, 0, 0, 0};

void HandlePumpTimer(int pumpNumber)
{
  if (relaysFlags[pumpNumber - 1] == 1)
  {
    if (millis() - pumpTimer[pumpNumber - 1] >= pumpTime[pumpNumber - 1])
    {
      pumpTimer[pumpNumber - 1] = millis();
      stopWater(pumpNumber);
    }
  }
}

//Set moisture value
void ReadMoisture()
{
  /**************These is for resistor moisture sensor***********
    float value1 = analogRead(A0);
    moisture1_value = (value1 * 120) / 1023; delay(20);
    float value2 = analogRead(A1);
    moisture2_value = (value2 * 120) / 1023; delay(20);
    float value3 = analogRead(A2);
    moisture3_value = (value3 * 120) / 1023; delay(20);
    float value4 = analogRead(A3);
    moisture4_value = (value4 * 120) / 1023; delay(20);
   **********************************************************/
  /************These is for capacity moisture sensor*********/
  float value1 = analogRead(moisture1);
  moisture1_value = map(value1, 590, 360, 0, 100);
  if (moisture1_value < 0) {
    moisture1_value = 0;
  }
  float value2 = analogRead(moisture2);
  moisture2_value = map(value2, 600, 360, 0, 100);
  if (moisture2_value < 0) {
    moisture2_value = 0;
  }
  float value3 = analogRead(moisture3);
  moisture3_value = map(value3, 600, 360, 0, 100);
  if (moisture3_value < 0) {
    moisture3_value = 0;
  }
  float value4 = analogRead(moisture4);
  moisture4_value = map(value4, 600, 360, 0, 100);
  if (moisture4_value < 0) {
    moisture4_value = 0;
  }
}
