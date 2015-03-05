#include <Arduino.h>
#include "panel.h"
static unsigned char recieve_buffer[8];
static unsigned char recieve_pointer;
static unsigned char data_length;
static unsigned char recieve_state = 0;
static unsigned char lastCmd;
static unsigned long oldKeyState;
static unsigned char x;
static unsigned char ledState;
#define recieve_state_waiting 0
#define recieve_state_cmd 1
#define recieve_state_len 2
#define recieve_state_data 3
#define recieve_state_data 4
#define SOH 0x01 //start byte
#define EOT 0x04 // stop byte
#define ui_port Serial1 //tx



void sendAck()
{
  ui_port.write(SOH);
  ui_port.write(0x06);
  ui_port.write(0x01);
  ui_port.write(lastCmd);
  ui_port.write((~0x06) + 1);
  ui_port.write(EOT);
}

void sendNack()
{
  ui_port.write(SOH);
  ui_port.write(0x15);
  ui_port.write(0x00);
  ui_port.write((~0x15) + 1);
  ui_port.write(EOT);
}

void writeLed(unsigned char led, unsigned char state)
{
  if (state)
  {
    writeLeds(ledState = ledState | 1 << led);
  }
  else
  {
    writeLeds(ledState & (~(1 << led)));
  }
}

void writeLeds(unsigned char leds)
{
  ledState = leds;
  ui_port.write(SOH);
  ui_port.write(0x31);
  ui_port.write(0x02);
  ui_port.write(leds & 255);
  ui_port.write(leds >> 8);
  ui_port.write((255 ^ 0x31) + 1);
  ui_port.write(EOT);
}

void writeKey(char key, unsigned char state)
{
  unsigned char x;
  switch (key)
  {
    case 6:
      x = 1;
      break;
    case 7:
      x = 0;
      break;
    case 9:
      x = 2;
      break;
    case 12:
      x = 3;
      break;
    case 10:
      x = 4;
      break;
    case 11:
      x = 6;
      break;
    case -10:
      x = 5;
      break;
    case -11:
      x = 7;
      break;
  }
  writeLed(x, state);
}
void setLedBacklight(unsigned char led, unsigned char brightness)
{
  ui_port.write(SOH);
  ui_port.write(0x32);
  ui_port.write(0x02);
  ui_port.write(led);
  ui_port.write(brightness);
  ui_port.write((255 ^ 0x32) + 1);
  ui_port.write(EOT);
}


void interpretWriteKeyStatus()
{
  unsigned char i;
  unsigned long newData;

  newData = *((unsigned long *)recieve_buffer);
  //Iterate over all bits in the key map thingy
  for (i = 0; i < 24; i++)
  {
    if ((oldKeyState ^ newData) & (1 << i))
    {
      if (newData & (1 << i))
      {
        onKeyPress(i);
      }
      else
      {
        onKeyUp(i);
      }
    }
  }

  oldKeyState = newData;
}

void pollUI()
{
  //No new data, go home
  if (!ui_port.available())
  {
    return;
  }

  //Recieving state machine
  switch (recieve_state)
  {
    //If we are waiting for a packet to start, then we start listening for the command byte, but
    //only if we have recieved the correct start byte
    case recieve_state_waiting:
      if (ui_port.read() == SOH)
      {
        recieve_pointer = 0;
        recieve_state = recieve_state_cmd;

      }
      return;

    //If waiting for the command, accept the command and wait for len
    case recieve_state_cmd:

      recieve_state = recieve_state_len;
      lastCmd = ui_port.read();
      return;

    //If waiting for len, accept the len and wait for data
    case recieve_state_len:

      recieve_state = recieve_state_data;
      data_length = ui_port.read();
      return;

    //If recieving data, either add the data to the buffer, or, if all bytes have been recived, verify the checksum, ack or nack as appropriate,
    //And do the commandRecieved() function if the checksum was correct.
    case recieve_state_data:

      if (recieve_pointer < data_length)
      {
        recieve_buffer[recieve_pointer] = ui_port.read();
        recieve_pointer ++;
        return;
      }
      else
      {
        // Serial.println((255^lastCmd)+1);
        if (ui_port.read() == (255 ^ lastCmd) + 1)
        {
          if (!((lastCmd == 0x15) || (lastCmd == 6)))
          {
            sendAck();
          }
          recieve_state = recieve_state_waiting;
          commandRecieved();
        }
        else
        {

          sendNack();
          recieve_state = recieve_state_waiting;
        }
      }
  }
}

void commandRecieved()
{

  if (lastCmd == 0x33)
  {
    interpretWriteKeyStatus();
  }
}
