#include <Wire.h>
int x = 0;

#define BUFFER_SIZE  8
#define READ_CYCLE_DELAY 1000

int slaveAddress[] = {9, 8};
byte buffer[BUFFER_SIZE];
void setup() {
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.begin(9600);
  Serial.println("MEGA IS READY...");
}
void loop() {
  //  Wire.beginTransmission(slaveAddress[1]); // transmit to device #9
  //  Wire.write(x);
  //  Wire.endTransmission();    // stop transmitting
  x++; // Increment x
  if (x > 5) x = 0; // `reset x once it gets 6
  delay(500);

  //RequestInfo(slaveAddress[0]);
  //RequestInfo(slaveAddress[1]);
  Serial.println("*************************");

  delay(READ_CYCLE_DELAY);

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
