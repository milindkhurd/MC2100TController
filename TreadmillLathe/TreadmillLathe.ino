#include <TimerOne.h>

/*
  MC-2100 Treadmill Motor Controller Interface
  Lathe Motor Controller via PWM
  ON/OFF Toggle

  Original from
  Joe Schoolcraft
  Brian Schoolcraft
  May 2013
  https://sonsofinvention.wordpress.com/2013/05/22/arduino-compatible-mc-2100-controller-and-lathe-tachometer

  Updates to make it work and add debounce FanMan 170122
 
*/


#define POT_READ A0 //Wiper of pot connected as voltage divider (Speed Command)
#define PWM_OUT 9 //Connected to blue wire of MC2100 (50ms period PWM out)
#define ON_OFF 12 //On/Off Switch Input
#define TO_LOW_DELAY 50 //Debounce time for HI to LO switch transition
#define TO_HIGH_DELAY 50 //Debounce time for LO to HI switch transition


#define PWM_CYCLE 50.0 //Output Signal PWM Period (50ms)

#define POT_DIF 4 //Change detection threshold on pot
#define MAX_DUTY 869 //Max Duty Cycle expected by MC-2100 (85% of 1023)
#define MIN_DUTY 0 //Min Duty Cycle expected by MC-2100 (0% of 1023)

int potTemp;
int potValue;
int lastPotValue;
int potCheck;
int speedLevel;

byte onOffState = 0;
byte lastonOffState = 0;
unsigned long lastOnOffTime = 0;

void setup()
{
  pinMode(POT_READ, INPUT);
  pinMode(PWM_OUT, OUTPUT);
  pinMode(ON_OFF, INPUT_PULLUP); //Enable internal pullup resistor to simplify external circuit

  Timer1.initialize(PWM_CYCLE * 1000); //Set pin 9 and 10 period to 50 ms
  Timer1.pwm(PWM_OUT, 25); //Start PWM at 0% duty cycle
  Serial.begin(9600);
}

void loop()
{
  //Read and condition pot value
  potTemp = analogRead(POT_READ);
  potCheck = abs(potTemp - potValue);
  if (potCheck >= POT_DIF) { //Only accept new value if it’s far enough from the current accepted value
    potValue = potTemp;
  }

  speedLevel = map(potValue, 0, 1023, 0, MAX_DUTY); //Convert Pot input to pwm level to send to MC-2100

  //Serial.println(speedLevel);

  onOffState = debounce(ON_OFF, &lastonOffState, &lastOnOffTime, TO_LOW_DELAY, TO_HIGH_DELAY);

  if (onOffState == LOW) { //Off  switch to ground is closed
    Timer1.setPwmDuty(PWM_OUT, 0); //Shut down MC-2100
  }

  if (onOffState == HIGH) { //ON switch to ground is open
    Timer1.setPwmDuty(PWM_OUT, speedLevel); //Send speed command to MC-2100
    //Serial.println(analogRead(9));
    Serial.println(speedLevel);
  }

}

////////////////////////////////////////////////////////////////////////////////////////////
/* Function for debouncing digital inputs

  Arguments:
  _debouncePin - ID of pin to be read/debounced
  lastReading - pointer to variable storing the previous reading (HIGH/LOW) of the input pin
  lastDebounceTime - pointer to variable storing the last time (ms) the input changed (not debounced)
  _toLowDelay - debounce time for HIGH to LOW transition
  _toHighDelay - debounce time for LOW to HIGH transition

  åReturns:
  _state - debounced state (HIGH/LOW) of _debouncePin
*/
////////////////////////////////////////////////////////////////////////////////////////////

byte debounce(byte _debouncePin, byte * lastReading, unsigned long * lastDebounceTime, int _toLowDelay, int _toHighDelay)
{
  byte _reading = digitalRead(_debouncePin);
  byte _state = *lastReading;

  if (_reading != *lastReading) { // pin state just changed
    *lastDebounceTime = millis(); // reset the debouncing timer
  }

  if ((millis() - *lastDebounceTime) >= _toLowDelay && _reading == LOW) {
    // whatever the reading is at, it's been there for longer
    // than the hold delay, so take it as the actual current state for use in the rest of the script
    _state = _reading;
    *lastReading = _reading;
    return _state;
  }

  if ((millis() - *lastDebounceTime) >= _toHighDelay && _reading == HIGH) {
    // whatever the reading is at, it's been there for longer
    // than the hold delay, so take it as the actual current state for use in the rest of the script
    _state = _reading;
    *lastReading = _reading;
    return _state;
  }
  *lastReading = _reading;
  return _state;
}
///////////////////////////////////////////////////////////////
