#include "pages/display_page_manager.h"
#include "pages/dpm_base_page.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>

DisplayPageManager PageManager;

ScreenConfig::ScreenConfig()
    : Flags(SSD1306_SWITCHCAPVCC)
    , I2CAddress(0x3C)
{
}

void DisplayPageManager::InitialiseDisplaySubsystem(int Width, int Height, TwoWire& I2CBus, ScreenConfig Config)
{
    if (!Display)
    {
        Display = new Adafruit_SSD1306(128, 64, &I2CBus, -1);
        Display->begin(Config.Flags, Config.I2CAddress);
	    Display->clearDisplay();
    }
}

ScreenPage& DisplayPageManager::RegisterRoot(ScreenPage& Page)
{
    if (LinkedList)
    {
        LinkedList = LinkedList->AddAsRootSet(Page);
    }
    else
    {
        LinkedList = &Page;
    }

    return Page;
}

void DisplayPageManager::Update(Keypad& keypad)
{
    if(CurrentPage == nullptr)
    {
        return;
    }

    if (!CurrentPage->IsPageFocused())
    {
        if (keypad.WasPressedEat(EPadButton::Up))
        {
            Previous();
        }
        
        if (keypad.WasPressedEat(EPadButton::Down))
        {
            Next();
        }

        if (keypad.WasPressedEat(EPadButton::Right))
        {
            CurrentPage->SetFocusMode(EFocusMode::PageFocus);
        }	
    }

    if (keypad.WasPressedEat(EPadButton::Left))
    {
        switch(CurrentPage->FocusMode)
        {
            case EFocusMode::ItemFocus:
                CurrentPage->SetFocusMode(EFocusMode::PageFocus);
                break;
            case EFocusMode::PageFocus:
                CurrentPage->SetFocusMode(EFocusMode::NoFocus);
                break;
            case EFocusMode::NoFocus:
                PopPage();
        }
    }
}

void DisplayPageManager::Next()
{
    if (LinkedList)
    {
        LinkedList = LinkedList->Next();
        PushPage(LinkedList, false);
    }
}

void DisplayPageManager::Previous()
{
    if (LinkedList)
    {
        LinkedList = LinkedList->Prev();
        PushPage(LinkedList, false);
    }
}

void DisplayPageManager::PushPage(ScreenPage* Page, bool bAutoFocus)
{
    PageRequest = Page;
    if (PageRequest->ForwardNode == nullptr)
    {
        Page->BackwardNode = CurrentPage;
    }

    if (bAutoFocus)
    {
        Page->SetFocusMode(EFocusMode::PageFocus);
    }
}

void DisplayPageManager::PopPage()
{
    if (CurrentPage->ForwardNode == nullptr)
    {
        PageRequest = CurrentPage->BackwardNode;
        PageRequest->SetFocusMode(EFocusMode::PageFocus);
        return;
    }
}

void DisplayPageManager::SelectPage()
{
    if (CurrentPage == nullptr && LinkedList != nullptr)
    {
        PageRequest = LinkedList->Next();
    }

    if (PageRequest != nullptr && CurrentPage != PageRequest)
    {
        if (CurrentPage)
        {
            CurrentPage->Deactivate();
        }

        CurrentPage = PageRequest;
        PageRequest = nullptr;
        CurrentPage->Activate();
    }
}

ScreenPage* DisplayPageManager::Current() 
{
    return CurrentPage;
}

