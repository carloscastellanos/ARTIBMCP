#include <Wire.h>
#include "Wire.h"

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

// --------------- I2C ------------
//                      pump &   activeTime
// Expecting to receive int char float
//                bytes: 4  1    4       = 9
//              example: 1&1000
#define BUFFER_SIZE 9
short data[BUFFER_SIZE];
int incomingMSG;

byte slaveAddress = 8;

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
}

void loop()
{
  HandlePumpTimer(1);
  HandlePumpTimer(2);
  HandlePumpTimer(3);
  HandlePumpTimer(4);
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
