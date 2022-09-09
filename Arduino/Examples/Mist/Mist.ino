byte mist = 6;
int releaseTime = 10000;  // Mist will be active during 10k ms
int waitingTime = 1000;   // After Mist is active, we wait 1k ms to enable it again

void setup() {
  // Initialize Pins
  pinMode(mist, OUTPUT);
}

void loop() {
  // Mist Routine
  digitalWrite(mist, HIGH);
  delay(releaseTime);
  digitalWrite(mist, LOW);
  delay(waitingTime);
}
