// Include the required Wire library for I2C<br>
#include <Wire.h>
int x = 0;

#define BUFFER_SIZE 2
short data[BUFFER_SIZE];

void setup() {
  // Start the I2C Bus as Slave on address 9
  Wire.begin(8);
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
}
void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  Serial.print("Something reveiced: ");
  Serial.println(x);
}

void requestEvent()
{
  data[0] = 7 * 10; // In order to use short, I multiple by 10
  data[1] = 22;
  Wire.write((byte*)data, BUFFER_SIZE * sizeof(short));  
}

void loop() {
  if (Serial.available() > 0) {
    char msg = Serial.read();
    if (msg == 'h') {
      Serial.println(msg);
    }
  }
}
