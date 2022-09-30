String msg;
char msgType;
float msgValue;

// PROTOCOL
// a=auxin/aba
// p=peg
// w=water
// l=light
// t=temperature

const char auxin = 'a';
const char peg = 'p';
const char water = 'w';
const char light = 'l';
const char temperature = 't';

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    msg = Serial.readStringUntil('\n');

    if (msg.length() > 0)
    {
      msgType = msg.substring(0, 1)[0];
      msgValue = msg.substring(1, msg.length()).toFloat();

      //Serial.println(msg + ", " + msgType + ": " + msgValue);
      switch (msgType) {
        case auxin:
          // statements
          break;
        case peg:
          // statements
          break;
        case water:
          // statements
          break;
        case light:
          // statements
          break;
        case temperature:
          // statements
          break;
        default:
          // statements
          break;
      }

      Serial.println("m 75");
      Serial.println("t 23");
      Serial.println("l 0");
    }
  }
}
