/**
\file   final.ino
\date   2023-30-11
author Andres Castro <andresfcastro@unicauca.edu.co> - Daniel Tucanes <danieltucanes@unicauca.edu.co> - Sebastian Pisso <jpisso@unicauca.edu.co>
@brief  Smart greenhouse Program.

\par Copyright
Information contained here in is proprietary to and constitutes valuable
confidential trade secrets of Unicauca, and
is subject to restrictions on use and disclosure.

\par
Copyright (c) Unicauca 2022. All rights reserved.

\par
The copyright notices above do not evidence any actual or
intended publication of this material.
******************************************************************************
*/

/********************************************//**
 *  Define global variables
 ***********************************************/
//PASSWORD
String password = "2345";


/********************************************//**
 *  AVERAGE control functions
 ***********************************************/
#include <Average.h>
// Reserve space for 5 entries in the temperature, humidity and ligh average bucket.
Average<float> ave1(5);
Average<float> ave2(5);
Average<float> ave3(5);

/********************************************//**
 *  LCD control functions
 ***********************************************/
#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/********************************************//**
 *  KEYPAD control functions
 ***********************************************/
#include <Keypad.h>


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {24, 26, 28, 30}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {32, 34, 36, 38}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/********************************************//**
 *  LED(RGB) control functions
 ***********************************************/
int R = 42;
int G = 44;
int B = 46;

/********************************************//**
 *  State Machine control functions
 ***********************************************/
#include "StateMachineLib.h"

/** An enum type. 
 *  define states in State Machine 
 */
enum State
{
  INICIAL = 0,
  BLOQUEADO = 1,
  MONITORTyH = 2,
  MONITORLUZ = 3,
  ALARMA = 4
};

/** An enum type. 
 *  define Inputs in State Machine 
 */
enum Input
{
  AVANZARP = 0,
  AVANZARN = 1,
  RETROCEDER = 2,
  RETROCEDERN = 5,
  DESCONOCIDO = 4,
};

/*! Stores last user input */
Input currentInput;
/*! Create new StateMachine 5 states and 8 transitions */
StateMachine stateMachine(5, 8);

/********************************************//**
 *  Temperature sensor control functions
 ***********************************************/
#include "DHT.h"

#define DHTPIN  7
#define DHTTYPE DHT11

DHT dhtObject(DHTPIN, DHTTYPE);

/********************************************//**
 *  PHOTORESISTOR sensor control functions
 ***********************************************/
#define LIGHTPIN  A0


/********************************************//**
 *  BUZZER control functions
 ***********************************************/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


// change this to make the song slower or faster
int tempo = 140;

// change this to whichever pin you want to use
int buzzer = 8;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  // We Wish You a Merry Christmas
  // Score available at https://musescore.com/user/6208766/scores/1497501
  
  NOTE_C5,4, //1
  NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
  NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
  NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
  NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
  NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
  NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,

  NOTE_F5,2,
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

/********************************************//**
 *  Asynchronous Task control functions
 ***********************************************/
#include "AsyncTaskLib.h"

void welcome();
String writepassword();
void trypassword();
void timeout5sec();
void taketemperature();
void timeout5sec2();
void compalarma();
void takelight();
void timeout6sec();
void timeout3sec();
void turnalarm();

AsyncTask contrasenia(100, trypassword);
AsyncTask tm5(5000, timeout5sec);
AsyncTask temperatura(100, true,taketemperature);
AsyncTask tm52(5000, timeout5sec2);
AsyncTask luz(100, true,takelight);
AsyncTask tm6(6000, timeout6sec);
AsyncTask alarm(100, turnalarm);
AsyncTask tm3(3000, timeout3sec);

void setup() {
  // put your setup code here, to run once:
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  // Print a message to the LCD.
  lcd.print("Welcome!");
  delay(1000);
  lcd.clear();

  Serial.begin(9600);

  //Starting State Machine
  Serial.println("Starting State Machine...");
  setup_State_Machine();  
  Serial.println("Start Machine Started");
  
  stateMachine.SetState(State::INICIAL, false, true);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Update Asynchronous Task
  contrasenia.Update();
  tm5.Update();
  temperatura.Update();
  tm52.Update();
  luz.Update();
  tm3.Update();
  alarm.Update();
  tm6.Update();
  
  // Update State Machine
  stateMachine.Update();
  //Serial.print("currentInput: ");
  //Serial.println(currentInput);
}

/**
* @name timeout5sec
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Define timeout5sec function for to leaf the state INICIAL
*----------------------------------------------------------------------------
*
*/
void timeout5sec(){
  Serial.println("Entra a primer timeout de 5 seg");
  currentInput = Input::RETROCEDER;
  lcd.clear();
}

/**
* @name timeout5sec2
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Define timeout5sec2 function for to leaf the state MONITORTyH 
*----------------------------------------------------------------------------
*
*/
void timeout5sec2(){
  Serial.println("Entra a segundo timeout de 5 seg");
  //Serial.println(stateMachine.GetState());
  currentInput = Input::AVANZARN;
}

/**
* @name timeout6sec
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Define timeout6sec function for to leaf the state ALARMA 
*----------------------------------------------------------------------------
*
*/
void timeout6sec(){
  Serial.println("Entra a timeout de 6 segundos");
  //Serial.println(stateMachine.GetState());
  currentInput = Input::RETROCEDERN;
}

/**
* @name timeout3sec
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Define timeout3sec function for to leaf the state MONITORLUZ 
*----------------------------------------------------------------------------
*
*/
void timeout3sec(){
  Serial.println("Entra a timeout de 3 segundos");
  //Serial.println(stateMachine.GetState());
  currentInput = Input::RETROCEDER;
}