byte heatPad_MOSFET_Pin = 3;

void setup() {
  // Initialize Pins
  pinMode(heatPad_MOSFET_Pin, OUTPUT);

  // Start SerialCOM for testing purposes,
  // so we can control the heatpad from the IDE Serial Monitor
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
    char msg = Serial.read();
    if(msg == 'a'){
      Serial.println("Heat ON");
      analogWrite(heatPad_MOSFET_Pin, 255);
    }
    else if(msg == 'b'){
      Serial.println("Heat OFF");
      analogWrite(heatPad_MOSFET_Pin, 0);
    }
  }
}
