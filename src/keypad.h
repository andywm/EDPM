#pragma once

enum class EPadButton
{
  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3,
  Select = 4,
  Cancel = 5,
  Aux1 = 6,
  Aux2 = 7,
};

struct Keypad
{
  int LastState = 0;
  int CurrentState = 0;
  int Latch = 0;
  int Clock = 0;
  int Data = 0;

  void Init(int Latch, int Clock, int Data);
  void UpdatePadState();

  bool IsButtonDown(EPadButton) const;
  bool WasButtonDown(EPadButton) const;

  bool WasPressed(EPadButton) const;
  bool WasPressedEat(EPadButton);
  bool WasReleased(EPadButton) const;
};