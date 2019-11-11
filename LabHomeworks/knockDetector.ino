const int passivePin = A0;
const int activePin = 9;
const int pushButton = 2;

int buttonState = 0;
int speakerValue = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

const int threshold = 71;

void setup() {
  pinMode(passivePin, INPUT);
  pinMode(pushButton, INPUT);
  pinMode(activePin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  speakerValue = analogRead(passivePin);
  if (speakerValue != 0) {
    Serial.println(speakerValue);
  }
  if (speakerValue > threshold) {
    currentMillis = millis();
    Serial.println("Knock");
    if(currentMillis - previousMillis >= 5000) {
      previousMillis = currentMillis;
      tone(activePin, 2000);
    }
  }
  buttonState = digitalRead(pushButton);
  if (buttonState == 1) {
    noTone(activePin);
  }
}
