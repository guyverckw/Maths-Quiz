/*
 * Maths test program with 4x4 Matrix Keypad connected to Arduino
This code prints the key pressed on the keypad to the serial port*/


// include MD_Parola library for scrolling text on LED Matrix
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// To support 4x4 Keypad for input
#include <Keypad.h>


// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define MAX_DEVICES 16
#define MAX_ZONES 2
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Setup parameters for MD_Parola
uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_PRINT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 2000; // in milliseconds

// Variables for Keypad
const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

long first_no = 0;
long sec_no = 0;
long opr = 0;
int answer = 0;
bool four_Q = false;

char keymap[numRows][numCols]=
{
{'4', '8', '#', 'D'},
{'3', '7', '*', 'C'},
{'2', '6', '0', 'B'},
{'1', '5', '9', 'A'}
};
//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {9,8,7,6}; //Rows 0 to 3
byte colPins[numCols]= {5,4,3,2}; //Columns 0 to 3

//Buffer to collect input
char buf[11];
char temp_Q[60];
int i = 0;
int ready_out = 0;

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

// HARDWARE SPI
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Global message buffers shared by Serial and Scrolling functions
#define  BUF_SIZE  75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Press # to start!!!!" };
bool newMessageAvailable = true;
bool to_zone = false;


//Generate Maths questions by using random generated no.
void question()
{
  randomSeed(analogRead(3) * millis());  
  char temp[10];
  
  first_no = random(999);
  sec_no = random(first_no);
  opr = random(0, 999); 

  //use newMessage to setup new question
  Serial.print("Question: ");
  strcpy(newMessage, "Q:  ");
  Serial.print(first_no);
  ltoa(first_no, temp, 10);
  strcat(newMessage, temp);
  switch (opr % 4)
  {
    case 0:     
      Serial.print(" + ");
      strcat(newMessage, "  + ");
      answer = first_no + sec_no;
      break;
    case 1:     
      Serial.print(" - ");
      strcat(newMessage, "  - ");
      answer = first_no - sec_no;
      break;
    case 2:     
      Serial.print(" + ");
      strcat(newMessage, "  + ");
      answer = first_no + sec_no;
      break;
    case 3:     
      Serial.print(" - ");
      strcat(newMessage, "  - ");
      answer = (long)(first_no - sec_no);
      break;
  }
  Serial.print(sec_no);
  ltoa(sec_no, temp, 10);
  strcat(newMessage, temp);
  Serial.println(" = ?");
  strcat(newMessage, " = ?");
  strcpy(temp_Q, newMessage);
  newMessageAvailable = true;
}

//Generate Maths questions by using random generated no.
void four_question()
{
  randomSeed(analogRead(3) * millis());  
  char temp[10];
  
  first_no = random(99);
  sec_no = random(first_no);
  opr = random(0, 999); 

  //use newMessage to setup new question
  Serial.print("Question: ");
  strcpy(newMessage, "Q:  ");
  Serial.print(first_no);
  ltoa(first_no, temp, 10);
  strcat(newMessage, temp);
  switch (opr % 4)
  {
    case 0:     
      Serial.print(" + ");
      strcat(newMessage, "  +  ");
      answer = first_no + sec_no;
      break;
    case 1:     
      Serial.print(" - ");
      strcat(newMessage, "  -  ");
      answer = first_no - sec_no;
      break;
    case 2:     
      Serial.print(" x ");
      strcat(newMessage, "  x  ");
      answer = first_no * sec_no;
      break;
    case 3:     
      Serial.print(" / ");
      strcat(newMessage, "  /  ");
      answer = (long)(first_no / sec_no);
      break;
  }
  Serial.print(sec_no);
  ltoa(sec_no, temp, 10);
  strcat(newMessage, temp);
  Serial.println(" = ?");
  strcat(newMessage, " = ?");
  strcpy(temp_Q, newMessage);
  newMessageAvailable = true;
}

void setup()
{
  Serial.begin(9600);
  Serial.print("\n[Parola Scrolling Display]\nType a message for the scrolling display\nEnd message line with a newline\n");

  P.begin(MAX_ZONES);

  P.setZone(0, 4, 15);
  P.setZone(1, 0, 3);
  P.setIntensity(0, 1);
  P.setIntensity(1, 1);
  
  P.displayClear(0);
  P.displayClear(1);


  P.displayZoneText(0, newMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  
  Serial.println("Press #/A to start");
  while (true)
  {
    if (P.displayAnimate())
    {
      if (P.getZoneStatus (0))
      {
        //P.displayReset(0);
        Serial.println("Setup Zone 0");
        
        P.displayZoneText(1, "", scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
        Serial.println("Setup Zone 1");
        //P.write("");
        P.displayReset(1);
        break;
      }
    } 

    
  }
  
  while (true)
  {
    char keyin = myKeypad.getKey();
    
    if (keyin == '#')
    {
      Serial.println("+ / - Q");
      question();
      four_Q = false;
      break;
    }
    else if (keyin == 'A')
    {
      Serial.println("+ - x /");
      four_question();
      four_Q = true;
      break;
    }
  }

}

//If key is pressed, this key is stored in 'keypressed' variable
//If key is not equal to 'NO_KEY', then this key is printed out
//if count=17, then count is reset back to 0 (this means no key is pressed during the whole keypad scan process
void loop()
{
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY)
  {
    if ((keypressed != '#') && (keypressed != 'A') && (keypressed != 'B') && (keypressed != 'C') && (keypressed != 'D'))
    {
      if (keypressed != '*')
      {
        Serial.print(keypressed);
        P.displayZoneText(1, buf, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
        P.displayReset(1);
        if (i < 9)
        {
          buf[i] = keypressed;
          i++; 
        }
        else if (i == 9)
        {
          buf[i] = keypressed;
          buf[i+1] = '\n';
          ready_out = 1;
          i = 0;
        }
      }
      else
      {
        buf[i] = '\n';
        ready_out = 1;
        i = 0;
      }
    }
  } 
  
  if (ready_out == 1)
  {
    //Serial.println(buf);
    if (answer == atol(buf))
    {
      Serial.println("\nCorrect!!!\nNext Question...\n");
      P.displayZoneText(1, "Correct!", scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
//    P.write("Correct! Next Question!");
      P.displayReset(1);
      delay(2000);
      if (four_Q)
      {
        four_question();
      }
      else
      {
        question();
      }
    }
    else
    {
      Serial.println("\nYou're wrong!!!  Try again!!!");
      Serial.println(temp_Q);
      P.displayZoneText(1, "Wrong!", scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
//      P.write("Wrong! Try again!");
      P.displayReset(1);
      delay(2000);
      strcpy(curMessage, temp_Q);
      newMessageAvailable = true;
    }
    ready_out = 0;
    for (int r = 0; r < 11; r++)
    {
      buf[r] = ' ';
      P.write("");
      P.displayReset();
    }
  } 
  
  if (P.displayAnimate())
  {
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      P.displayZoneText(0, curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
      P.displayZoneText(1, "", scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
      newMessageAvailable = false;
    }
    P.displayReset(0);
    P.displayReset(1);
  }
  
}  
