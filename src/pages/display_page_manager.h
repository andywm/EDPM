#pragma once
#include <stdint.h>

namespace arduino
{
    class MbedI2C;
};
using TwoWire = arduino::MbedI2C;

class ScreenPage;
class Adafruit_SSD1306;
struct Keypad;

struct ScreenConfig
{
	ScreenConfig();
	uint8_t Flags; //SSD1306_SWITCHCAPVCC
	uint8_t I2CAddress;// = 0x3C; 
};

struct DisplayPageManager
{
    Adafruit_SSD1306* Display = nullptr;
	ScreenPage* LinkedList = nullptr;
	ScreenPage* CurrentPage = nullptr;
	ScreenPage* PageRequest = nullptr;

    void InitialiseDisplaySubsystem(int Width, int Height, TwoWire& I2CBus, ScreenConfig Config = ScreenConfig());

	ScreenPage& RegisterRoot(ScreenPage& Page);

	void Update(Keypad& keypad);
	void Next();
	void Previous();

	void PushPage(ScreenPage* Page, bool bAutoFocus = true);
	void PopPage();

	void SelectPage();

	ScreenPage* Current();
};

extern DisplayPageManager PageManager;
