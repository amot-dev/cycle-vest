//constant containing the night light pin number
const int PIN_NIGHT_LED = 2;

//initialize variable to hold photoresistor analogue value (0 - 1023)
int photoResistorAnalogue = 0;

//initialize value to hold the current time in milliseconds
unsigned long currentTime = 0;

//class for each set of lights (except night lights)
class LightSet{
private:
  //the delay between blinks
  const static int BLINK_DELAY = 350;
  
  //pin numbers
  int LEDPin;
  int buttonPin;
  
  //contains the current state of the button (pressed or not)
  byte buttonState = LOW;
  //contains the current state of the LEDs (on or off)
  byte lightState = LOW;
  //contains the blinking state of the LEDs (blinking or not)
  bool blinking = false;
  
  //contains the last time the blinking lights were actuated
  unsigned long lastBlinkerUpdateTime = 0;
  
  //updates the lights (this function is mainly here to make the code easier to read)
  void updateLEDs();

public:
  //constructor (sets pin numbers)
  LightSet(int LEDPin, int buttonPin);
  void initializePins();
  void updateButtonState();
  void updateBrakeLights();
  void updateBlinkerLights();
};

LightSet::LightSet(int outputPin, int inputPin){
  LEDPin = outputPin;
  buttonPin = inputPin;
};
void LightSet::initializePins(){
  pinMode(LEDPin, OUTPUT);
  pinMode(buttonPin, INPUT);
};
void LightSet::updateLEDs(){
  digitalWrite(LEDPin, lightState);
};
void LightSet::updateButtonState(){
  //read state from button
  buttonState = digitalRead(buttonPin);
};
void LightSet::updateBrakeLights(){
  //if the button is pressed
  if (buttonState == HIGH) {
    //if the lights are on, turn them off
    if (lightState == HIGH) lightState = LOW;
    //if the lights are off, turn them on
    else lightState = HIGH;
    updateLEDs();
  }
};
void LightSet::updateBlinkerLights(){
  Serial.println("Button state is: ");
  Serial.println(buttonState);
  Serial.println("Blinking state is: ");
  Serial.println(blinking);
  Serial.println("LED state is: ");
  Serial.println(lightState);
  //if the button is pressed
  if (buttonState == HIGH){
    //if the lights are blinking
    if (blinking){
      //stop the blinking
      blinking = false;
      //if the lights are on, turn them off
      if (lightState == HIGH){
        lightState = LOW;
        updateLEDs();
      }
    }
    //if the lights are not blinking
    else {
      //begin blinking
      blinking = true;
      //turn the lights on
      lightState = HIGH;
      updateLEDs();
    }
  }
  //regular course of action (button not pressed)
  else {
    //if elapsed time is not less than the delay between blinks and the lights are supposed to be blinking
    if (((currentTime - lastBlinkerUpdateTime) >= BLINK_DELAY) && blinking) {
      //if the lights are on, turn them off
      if (lightState == HIGH) lightState = LOW;
      //if the lights are off, turn them on
      else lightState = HIGH;
      updateLEDs();
    }
  }
};

//create light set objects (output, input)
LightSet LeftArmBrakes(8, 9);
LightSet LeftArmBlinkers(4, 3);

void setup() {
  // initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  delay(500);
  Serial.println("Arduino Online.");
  //initialize pins
  LeftArmBrakes.initializePins();
  LeftArmBlinkers.initializePins();
  pinMode(PIN_NIGHT_LED, OUTPUT);
}

void loop() {
  //get current time
  currentTime = millis();
  
  //LEFT ARM BLINKERS
  LeftArmBlinkers.updateButtonState();
  LeftArmBlinkers.updateBlinkerLights();
  
  //LEFT ARM BRAKES
  LeftArmBrakes.updateButtonState();
  LeftArmBrakes.updateBrakeLights();
  
  //NIGHT LIGHT
  //read value from photoresistor
  photoResistorAnalogue = analogRead(A0);
  Serial.println(photoResistorAnalogue);
  //if the light level is low, turn on the night light
  if (photoResistorAnalogue < 750) digitalWrite(PIN_NIGHT_LED, HIGH);
  //if the light level is high, turn off the night light
  else digitalWrite(PIN_NIGHT_LED, LOW);
}
