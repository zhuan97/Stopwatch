/*
 Date: 12/4/2018
 Programmers: Zhuan Chen, Robert Guillermo
 Name: stopwatch created with arduino
 */
 
/////////////////////////////////////////////4 Digits 7 segment Diagram//////////////////////////////////////////// segments in one digit////////////////
////                                                                                       ////////                         A                        ////
////         pins:                        9   2   7   10   1   3                           ////////                         |                        ////
////  segment or digit:                   1   A   F   2    3   B                           ////////                       _____                      ////
////                                      |   |   |   |    |   |                           ////////                      |     |                     ////                      
////                                   ----------------------------                        ////////                  F - |     | - B                 ////
////                                  |   |ˉ|  |ˉ|  |ˉ|  |ˉ|   |                        ////////                      ------                     ////
////                                  |   |ˉ| .|ˉ| .|ˉ| .|ˉ|.  |                        ////////                      |  G  |                    //// 
////                                   ----------------------------                        ////////                  E   |     | - C                ////               
////                                     |    |   |   |    |   |                           ////////                       ˉˉˉ                     ////
////  segment or digit:                  E    D   .   C    G   4                           ////////                         |                       ////
////         pins:                       6    5       4    8   12                          ////////                         D                       ////
////                                                                                       ////////                                                 ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Chrono.h> //Include the stopwatch library

//Declare and initlize some variables
int digit1 = 9;  //PWM Display pin 1
int digit2 = 10; //PWM Display pin 2
int digit3 = 11; //PWM Display pin 6
int digit4 = 12; //PWM Display pin 8

int segA = 2; //Display pin 14
int segB = 3; //Display pin 16
int segC = 4; //Display pin 13
int segD = 5; //Display pin 3
int segE = 6; //Display pin 5
int segF = 7; //Display pin 11
int segG = 8; //Display pin 15
int pauseButton = 13;    //Pause button pin digit 13                      
int resetButton = A0;    //Reset button pin analog A0
int relayState = HIGH;   //Set relayState as High, therefore the timer will be paused when it connects with the power                     
int pause; 
int reset;            
int pauseState;                 
int lastPauseState = LOW;                 
long lastDebounceTime = 0;                  
long debounceDelay = 100;   
unsigned long displayTimer;

Chrono libraryTimer;//Declare and initlize a library timer

void setup() {                
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);

  pinMode(pauseButton,INPUT);
  pinMode(resetButton,INPUT);
}

void loop() {
//initialize the two buttons
pause = digitalRead(pauseButton); //Read the pause button
reset = analogRead(resetButton);  //Read the reset button

//Every first press on the pause button will pause the timer and the second press will restart
//If the pause button is pressed
  if (pause != lastPauseState) 
  {   
    lastDebounceTime = millis();
  } 
    if ((millis() - lastDebounceTime) > debounceDelay) 
    {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
      if (pause != pauseState) 
      {
        pauseState = pause;
        // only pause the timmer if the new button state is HIGH
        if (pauseState == HIGH) 
        {
         relayState = !relayState;
         }
        }
      }

//The timer that goes every second and will be pause if the pause button is pressed
//Create the display timer by using the library timer
if (libraryTimer.hasPassed(1000)&& (pause == relayState) ) { 
    libraryTimer.restart(); // restart the library timer for every 1000ms

    displayTimer += 1; // for 1000ms passed on the library timer, the display timer will add 1 second
}

//If the reset button is pressed
if (reset == LOW ){
displayTimer = 0; // Reset the timer to 0
relayState = LOW; // After reset the number, pause the timer
}

displayNumber(displayTimer + (int)(displayTimer/60)*40); //Display the number and when it reach 60 seconds convert to 1 minute
lastPauseState = pause;  // Save the pause button reading.  Next time through the loop, it'll be the lastPauseState:
}


/////////////////////////////////////////////////Display Method//////////////////////////////////////////////////////////////////////
//Given a number, we display 10:22
//After running through the 4 numbers, the display is left turned off

//Display brightness
//Each digit is on for a certain amount of microseconds
//Then it is off until we have reached a total of 20ms for the function call
//Let's assume each digit is on for 1000us
//Each digit is on for 1ms, there are 4 digits, so the display is off for 16ms.
//That's a ratio of 1ms to 16ms or 6.25% on time (PWM).
//Let's define a variable called brightness that varies from:
//5000 blindingly bright (15.7mA current draw per digit)
//2000 shockingly bright (11.4mA current draw per digit)
//1000 pretty bright (5.9mA)
//500 normal (3mA)
//200 dim but readable (1.4mA)
//50 dim but readable (0.56mA)
//5 dim but readable (0.31mA)
//1 dim but readable in dark (0.28mA)

void displayNumber(int toDisplay) {
#define DISPLAY_BRIGHTNESS  2000 // Define the brightness of the display

#define DIGIT_ON  LOW
#define DIGIT_OFF  HIGH

  long beginTime = millis();//Declare the Arduino timer

  //For loop for each digit
  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      break;
    case 2:
      digitalWrite(digit2, DIGIT_ON);
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      break;
    }

    //Turn on the right segments for this digit
    //Display the correct number for each digit
    lightNumber(toDisplay % 10);
    toDisplay /= 10;

    delayMicroseconds(DISPLAY_BRIGHTNESS); 
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
  }

  while( (millis() - beginTime) < 10) ; 
  //Wait for 20ms to pass before we paint the display again
}

//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay) {

#define SEGMENT_ON  HIGH
#define SEGMENT_OFF LOW

  switch (numberToDisplay){

  case 0://If the number is 0
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 1://If the number is 1
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 2://If the number is 2
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 3://If the number is 3
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 4://If the number is 4
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 5://If the number is 5
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 6://If the number is 6
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 7://If the number is 7
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 8://If the number is 8
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 9://If the number is 9
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 10://If the number is 10
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;
  }

}
