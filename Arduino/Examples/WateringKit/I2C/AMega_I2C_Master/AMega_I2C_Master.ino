#include <Wire.h>
int x = 0;

#define BUFFER_SIZE  8
#define READ_CYCLE_DELAY 1000

int slaveAddress[] = {8};
byte buffer[BUFFER_SIZE];

boolean canRead = false;

void setup() {
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.begin(9600);
  Serial.println("MEGA IS READY...");
}
void loop() {

  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd == 'i') {
      canRead = !canRead;
    }
    if (cmd == '1') {
      SendCommand(slaveAddress[0], 1);
    }
    if (cmd == '2') {
      SendCommand(slaveAddress[0], 2);
    }
    if (cmd == '3') {
      SendCommand(slaveAddress[0], 3);
    }
    if (cmd == '4') {
      SendCommand(slaveAddress[0], 4);
    }
  }

  if (canRead) {
    RequestInfo(slaveAddress[0]);
    Serial.println("*************************");

    delay(READ_CYCLE_DELAY);
  }
}

void SendCommand(int slaveAdd, int pompNumber) {
  Wire.beginTransmission(slaveAdd);
  Wire.write(pompNumber);
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
    int moisture1_value = short_cast.val;

    // Parse the moisture
    short_cast.b[0] = buffer[2];
    short_cast.b[1] = buffer[3];
    int moisture2_value = short_cast.val;

    short_cast.b[0] = buffer[4];
    short_cast.b[1] = buffer[5];
    int moisture3_value = short_cast.val;

    short_cast.b[0] = buffer[6];
    short_cast.b[1] = buffer[7];
    int moisture4_value = short_cast.val;

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
