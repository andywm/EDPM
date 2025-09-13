#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
//#include "oled_interface.h"
#include "keypad.h"

//#include "pages/dpm_base_page.h"
#include "pages/display_page_manager.h"
#include "pages/dpm_interact_page.h"
#include "pages/dpm_info_page.h"

#define I2C0_SDA 2
#define I2C0_SCL 3
#define SWITCH 4

TwoWire I2CBus {I2C0_SDA, I2C0_SCL};


struct Example : public InfoTextPage
{
	PageInt Test1;
	PageInt Test2;

	Example()
	{
		//_________"12345678901234567890"
		SetLine<0>("This is only a test.");
		SetLine<1>("ValueA #", Test1);
		SetLine<2>("ValueB #", Test2);
		
		Test1 = 5;
		Test2 = 2;
	}
} TestPage1;


struct Example2 : public InfoTextPage
{
	PageInt Test1;
	PageInt Test2;

	Example2()
	{
		//_________"12345678901234567890"
		SetLine<0>("Second Page Test.");
		SetLine<1>("ValueA #. B ##", Test1, Test2);
		
		Test1 = 5;
		Test2 = 2;
	}
} TestPage2;

struct Example3 : public InteractivePage
{
	enum ESubPages : uint8_t
	{
		kPage1,
		PageCount,
	};
	ChildPageArray<PageCount> ChildPages {*this};
	PageInt ValueTest;

	static void OnAction()
	{
		//static bool bLightMode = false;
		//bLightMode = !bLightMode;
		digitalWrite(12, true);
	}

	Example3()
	{
		//_________"12345678901234567890"
		SetLine<0>("Third Page Test.");
		SetLineWithSubpage<1, kPage1>(">> Child Page");
		SetLineWithVar<2>("Val >###<", ValueTest);
		SetLineAsButton<3>("Button", &Example3::OnAction);
		
		ValueTest = 0;
	}
} TestPage3;

struct ExampleChild : public InfoTextPage
{
	ExampleChild()
	{
		//_________"12345678901234567890"
		SetLine<0>("Child Page.");
	}
} TestPageChild1;


Keypad keypad;

void Test()
{

}

void setup()
{
	keypad.Init();

	pinMode(12, OUTPUT);
	digitalWrite(12, false);

	PageManager.InitialiseDisplaySubsystem(128, 64, I2CBus);

	GetAllocationsTable().AddCallback(&Example3::OnAction);
	GetAllocationsTable().CallbackTable[0] = &Example3::OnAction;
	//AllocationsTable.CallbackTable[0] = &Example3::OnAction;

	PageManager.RegisterRoot(TestPage1);
	PageManager.RegisterRoot(TestPage2);
	PageManager.RegisterRoot(TestPage3)
		.AddChild(TestPageChild1, Example3::kPage1);	
}

void loop()
{
	keypad.UpdatePadState();
	PageManager.SelectPage();

	if(ScreenPage* Page = PageManager.Current())
	{
		PageManager.Update(keypad);
		Page->Update(keypad);
		Page->Draw();
	}
}
