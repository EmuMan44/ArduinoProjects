#include <Wire.h>
#include "AdafruitLEDPanel.h"

#define I2C_SLAVE_ADDRESS   0x08
#define CMD_BUFF_LEN        256

#define PANEL_WIDTH         64
#define LINES_ARRAY_SIZE    4
#define CMD_END_CHAR        '~'

//#define DEBUG
//#define TEST_PARSER
#define RECTANGULAR_TEXT
//#define HORIZONTAL_TEXT
#define ERASE_SCREEN        true

#define CLK                 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT                 10
#define OE                  9
#define A                   A0
#define B                   A1
#define C                   A2
#define D                   A3

AdafruitLEDPanel *panel;
volatile char command[CMD_BUFF_LEN];
volatile int cmdLen = 0;
volatile int loopDelay = 0;

/**
 * This method is called once during start up to initialize the device.
 */
void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  panel = new AdafruitLEDPanel(A, B, C, D, CLK, LAT, OE, true, PANEL_WIDTH, LINES_ARRAY_SIZE);
#ifdef TEST_PARSER
  processCommand("setTextLine 0 0 0 800 0 1 -1 Testing command parser.");
#endif
#ifdef RECTANGULAR_TEXT
  panel->setTextLine(0, "Normal landscape", 0, 0, panel->color(50, 0, 0), 0, 1, -1);
  panel->setTextLine(1, "Clockwise portrait", 0, 0, panel->color(0, 50, 0), 1, 1, -1);
  panel->setTextLine(2, "Inverted landscape", 0, 0, panel->color(0, 0, 50), 2, 1, -1);
  panel->setTextLine(3, "Anticlockwise portrait", 0, 0, panel->color(0, 50, 50), 3, 1, -1);
#else
  #ifdef HORIZONTAL_TEXT
  panel->setTextLine(0, "Font Size 1: Left", 0, 0, panel->color(50, 0, 0), 0, 1, -1);
  panel->setTextLine(1, "Font Size 1: Right", 0, 8, panel->color(0, 50, 0), 0, 1, 1);
  panel->setTextLine(2, "Font Size 2: Invert", 0, 0, panel->color(0, 0, 50), 2, 2, 1);
  #endif
#endif
}

/**
 * This method is called repeatedly to run the text display task.
 */
void loop()
{
  panel->displayTextTask(ERASE_SCREEN);
  if (loopDelay > 0) delay(loopDelay);
}

/**
 * This method is called when receiving a command from the I2C master. A command longer than 32 bytes will be split into
 * several I2C bus transfers. So this method could be called several times in order to reassemble a complete command.
 * The end of the command is indicated by the '~' character.
 *
 * @param numBytes specifies the number of bytes in the command.
 */
void receiveEvent(int numBytes)
{
#ifdef DEBUG
  Serial.print("*** Received bytes: ");
  Serial.println(numBytes);
#endif
  //
  // Keep reading until the command buffer is full or no more bytes in this transfer.
  //
  while (cmdLen < CMD_BUFF_LEN && Wire.available() > 0)
  {
    char ch = Wire.read();

    if (ch == CMD_END_CHAR)
    {
      //
      // We found the end-of-command character, let's process the command.
      //
      command[cmdLen] = '\0';
      processCommand(command);
      cmdLen = 0;
      break;
    }
    //
    // Add the character to the end of the command buffer.
    //
    command[cmdLen] = ch;
    cmdLen++;
  }

  if (cmdLen == CMD_BUFF_LEN)
  {
    //
    // Command size exceeds maximum, flush the data and abandon the invalid buffer.
    //
    while (Wire.available() > 0)
    {
      Wire.read();
    }
    cmdLen = 0;
  }
} //receiveEvent

/**
 * This method processes the command received.
 *
 * @param command specifies the command to be processed.
 */
void processCommand(char *command)
{
#ifdef DEBUG
  Serial.print("Command: <");
  Serial.print(command);
  Serial.println(">");
#endif

  char *pch = strtok(command, " ");
  if (pch != NULL)
  {
    if (strcmp(pch, "setTextLine") == 0)
    {
      int index = 0;
      char *text = NULL;
      int16_t x = 0, y = 0;
      uint16_t fontColor = 0;
      uint8_t rotation = 0, fontSize = 0;
      int16_t scrollInc = 0;

      if ((pch = strtok(NULL, " ")) != NULL)
      {
        index = atoi(pch);
#ifdef DEBUG
        Serial.print("index = ");
        Serial.println(index);
#endif
      }

      if ((pch = strtok(NULL, " ")) != NULL)
      {
        x = atoi(pch);
#ifdef DEBUG
        Serial.print("x = ");
        Serial.println(x);
#endif
      }

      if ((pch = strtok(NULL, " ")) != NULL)
      {
        y = atoi(pch);
#ifdef DEBUG
        Serial.print("y = ");
        Serial.println(y);
#endif
      }

      if ((pch = strtok(NULL, " ")) != NULL)
      {
        fontColor = atoi(pch);
#ifdef DEBUG
        Serial.print("fontColor = ");
        Serial.println(fontColor);
#endif
      }

      if ((pch = strtok(NULL, " ")) != NULL)
      {
        rotation = atoi(pch);
#ifdef DEBUG
        Serial.print("rotation = ");
        Serial.println(rotation);
#endif
      }

      if ((pch = strtok(NULL, " ")) != NULL)
      {
        fontSize = atoi(pch);
#ifdef DEBUG
        Serial.print("fontSize = ");
        Serial.println(fontSize);
#endif
      }

      if ((pch = strtok(NULL, " ")) != NULL)
      {
        scrollInc = atoi(pch);
#ifdef DEBUG
        Serial.print("scrollInc = ");
        Serial.println(scrollInc);
#endif
      }

      if ((pch = strtok(NULL, "\0")) != NULL)
      {
        text = pch;
#ifdef DEBUG
        Serial.print("text = <");
        Serial.print(text);
        Serial.println(">");
#endif
      }

      if (text != NULL)
      {
        panel->setTextLine(index, text, x, y, fontColor, rotation, fontSize, scrollInc);
      }
    }
    else if (strcmp(pch, "clearTextLine") == 0)
    {
      if ((pch = strtok(NULL, " ")) != NULL)
      {
        panel->clearTextLine(atoi(pch));
      }
    }
    else if (strcmp(pch, "clearAllTextLines") == 0)
    {
      for (int i = 0; i < LINES_ARRAY_SIZE; i++)
      {
        panel->clearTextLine(i);
      }
    }
    else if (strcmp(pch, "setDelay") == 0)
    {
      if ((pch = strtok(NULL, " ")) != NULL)
      {
        loopDelay = atoi(pch);
      }
    }
  }
} //processCommand

