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


#define Q_UP 7 //read for speed increment (Speed Command)
#define Q_DN 8 //read for speed decrement (Speed Command)

#define PWM_OUT 9 //Connected to blue wire of MC2100 (50ms period PWM out)
#define PWM_CYCLE 50.0 //Output Signal PWM Period (50ms)
#define BUTTON_DIFF 1 //Change detection threshold on clicks
#define MAX_DUTY 869 //Max Duty Cycle expected by MC-2100 (85% of 1023)
#define MIN_DUTY 0 //Min Duty Cycle expected by MC-2100 (0% of 1023)



//int highVal;
//int lowVal;
int buttonValue;
int speedLevel;

void setup()
{
  pinMode(Q_UP, INPUT);
  pinMode(Q_DN, INPUT);
  pinMode(PWM_OUT, OUTPUT);
 
  Timer1.initialize(PWM_CYCLE * 1000); //Set pin 9 and 10 period to 50 ms
  speedLevel = 0;
  Timer1.pwm(PWM_OUT, speedLevel); //Start PWM at 0% duty cycle
  //Serial.begin(9600);
}

void loop()
{

  if (speedLevel > 225)
    delay(10); //delay 10 secs so that we do not go full speed
  
  buttonValue = digitalRead(Q_UP);  // read input value
 
  if (buttonValue == LOW && speedLevel < MAX_DUTY) 
  {     
//  highVal = 0;
    speedLevel = speedLevel + BUTTON_DIFF;
    //Serial.println(speedLevel);
  }
//  else if (buttonValue == HIGH)
//   highVal = 1;


  buttonValue = digitalRead(Q_DN);  // read input value
    
  if (buttonValue == LOW && speedLevel > MIN_DUTY) 
  {
//   lowVal = 0;
     speedLevel = speedLevel - BUTTON_DIFF; 
  }
//  else if(buttonValue == HIGH)
//   lowVal = 1;

  Timer1.setPwmDuty(PWM_OUT, speedLevel); //Send speed command to MC-2100
  
}
