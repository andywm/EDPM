#include "keypad.h"
#include <Arduino.h>

constexpr int Latch = 14; 
constexpr int Clock = 15; 
constexpr int Data = 13; 

void Keypad::Init()
{
  pinMode(Latch, OUTPUT);
  pinMode(Clock, OUTPUT);
  pinMode(Data, INPUT);

  digitalWrite(Latch, HIGH);
  digitalWrite(Clock, LOW);
}

void Keypad::UpdatePadState()
{
  digitalWrite(Latch, LOW);
  digitalWrite(Latch, HIGH);

  LastState = CurrentState;
  CurrentState = 0;
  
  for (int Bit=0; Bit<8; Bit++)
  {
    int State = digitalRead(Data);
    CurrentState |= State << Bit;
    digitalWrite(Clock, HIGH);
    digitalWrite(Clock, LOW);
  }
}

bool Keypad::IsButtonDown(EPadButton Button) const
{
    return CurrentState & 0x1 << (int)Button;
}

bool Keypad::WasButtonDown(EPadButton Button) const
{
    return LastState & 0x1 << (int)Button;
}

bool Keypad::WasPressed(EPadButton Button) const
{
  return IsButtonDown(Button) && !WasButtonDown(Button);
}

bool Keypad::WasPressedEat(EPadButton Button)
{
  if (WasPressed(Button))
  {
    LastState |= 0x1 << (int)Button;
    return true;
  }
  return false;
}

bool Keypad::WasReleased(EPadButton Button) const
{
  return !IsButtonDown(Button) && WasButtonDown(Button);
}

