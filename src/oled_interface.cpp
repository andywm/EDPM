#include "oled_interface.h"
#include "keypad.h"
#include <Adafruit_SSD1306.h>

/*
InfoTextPage::DebugData InfoTextPage::IntegerProxy::NotSet;
Adafruit_SSD1306* Display = nullptr;
ScreenPageManager PageManager;

ScreenConfig::ScreenConfig()
    : Flags(SSD1306_SWITCHCAPVCC)
    , I2CAddress(0x3C)
{
}

void InitialiseDisplaySubsystem(int Width, int Height, TwoWire& I2CBus, ScreenConfig Config)
{
    if (!Display)
    {
        Display = new Adafruit_SSD1306(128, 64, &I2CBus, -1);
        Display->begin(Config.Flags, Config.I2CAddress);
	    Display->clearDisplay();
    }
}

Adafruit_SSD1306& ScreenPage::GetDisplay()
{
    return *Display;
}

int InfoTextPage::GetBGColour()
{
    return BLACK;
}

int InfoTextPage::GetFGColour()
{
    return WHITE;
}

void InfoTextPage::FormatInteger(char* Buffer, int BufferLength, int16_t Value)
{
    if (BufferLength == 0)
    {
        return;
    }

    if (Value < 0)
    {
        Buffer[0] = '-';
        if (BufferLength == 1)
        {
            Buffer[0] = '*';
            return;
        }

        Value = -Value;
        BufferLength--;
        Buffer++;
    }

    const uint8_t Magnitude = (Value >= 10) + (Value >= 100) + (Value >= 1000) + (Value >= 10000);

    if ((Magnitude + 1) > BufferLength)
    {
        std::memset(Buffer, '*', BufferLength);
        return;
    }
    
    //fill leading characters.
    std::memset(Buffer, ' ', BufferLength);
    constexpr int Ascii0 = 48;

    Buffer += sizeof(char) * (BufferLength - (Magnitude+1));

    int16_t ByReduction = Value;
    for (int16_t Order = pow(10, Magnitude); Order!=0; Order/=10)
    {
        int Count = 0;
        while (ByReduction / Order > 0)
        {
            ByReduction -= Order;
            Count++;
        }
        
        Buffer[0] = Ascii0 + Count;
        Buffer++;
    }
}

void InfoTextPage::Activate() 
{
    for (int Variable=0; Variable<AllocatedDebugVars; ++Variable)
    {
        const DebugData& Data = Allocations[Variable];
        FormatInteger(Data.Buffer, Data.Len, Data.Value);
    }

    GetDisplay().setTextSize(1);

    for (uint8_t Line = 0; Line < 4; ++Line)
    {
        DrawStaticLineElements(Line);
    }

    GetDisplay().display();
}

void InfoTextPage::Deactivate()
{
    GetDisplay().clearDisplay();		
}

void InfoTextPage::SetColourModeFromLine(uint8_t Line, EColourMode Threshold)
{
    int FgColour = GetFGColour();
    int BgColour = GetBGColour();

    if (Lines[Line].ColourFlags >= Threshold)
    {
        const int TempColour = FgColour;
        FgColour = BgColour;
        BgColour = TempColour;
    }

    GetDisplay().setTextColor(FgColour, BgColour);
}

void InfoTextPage::DrawStaticLineElements(uint8_t Line)
{
    SetColourModeFromLine(Line, EColourMode::INVERT);
    GetDisplay().setCursor(0, TextConstants::FontHeight*Line);
    GetDisplay().println(Lines[Line].Text);		
}

void InfoTextPage::MarkLineVarsDirty(uint8_t Line)
{
    for (int Variable = 0; Variable < AllocatedDebugVars; ++Variable)
    {
        DebugData& Data = Allocations[Variable];
        if (Data.Line == Line)
        {
            Data.bRedraw = true;
        }
    }
}

bool InfoTextPage::DrawVariables()
{
    bool bCommitRedraw = false;

    for (int Variable = 0; Variable < AllocatedDebugVars; ++Variable)
    {
        const DebugData& Data = Allocations[Variable];
        if (Data.bRedraw)
        {
            bCommitRedraw = true;

            SetColourModeFromLine(Data.Line, EColourMode::INVERT_VARS);
            FormatInteger(Data.Buffer, Data.Len, Data.Value);
            
            for (int CharOffset = 0; CharOffset < Data.Len; ++CharOffset)
            {
                const int XPixel = (Data.Cursor + CharOffset) * TextConstants::FontWidth;
                const int YPixel = TextConstants::FontHeight*Data.Line;
                GetDisplay().setCursor(XPixel, YPixel);
                GetDisplay().print(Data.Buffer + sizeof(char) * CharOffset);
            }
        }
    }

    return bCommitRedraw;
}

void InfoTextPage::Draw()
{
    if (DrawVariables())
    {
        GetDisplay().display();
    }
}
*/

/*
void InteractivePage::Activate()
{
    InfoTextPage::Activate();
}

void InteractivePage::Deactivate()
{
    InfoTextPage::Deactivate();
    ObservedFocusMode = EFocusMode::NoFocus;
    CurrentLine = 0;
}

void InteractivePage::ScrollDown()
{
    PreviousLine = CurrentLine;
    if (CurrentLine < MaxLine)
    {
        CurrentLine++;
    }
    else
    {
        CurrentLine = 0;
    }
}

void InteractivePage::ScrollUp()
{
    PreviousLine = CurrentLine;
    if (CurrentLine > 0)
    {
        CurrentLine--;
    }
    else
    {
        CurrentLine = MaxLine;
    }
}

void InteractivePage::EditValueDown()
{
    if (CurrentLine != UINT8_MAX && Lines[CurrentLine].VarEditIdx != UINT8_MAX )
    {
        DebugData& Data= Allocations[Lines[CurrentLine].VarEditIdx];
        Data.bRedraw = true;
        Data.Value--;
    }
}

void InteractivePage::EditValueUp()
{
    if (CurrentLine != UINT8_MAX && Lines[CurrentLine].VarEditIdx != UINT8_MAX )
    {
        DebugData& Data= Allocations[Lines[CurrentLine].VarEditIdx];
        Data.bRedraw = true;
        Data.Value++;
    }
}

void InteractivePage::Draw()
{
    bool bNeedClearPrevLine = PreviousLine != UINT8_MAX && PreviousLine != CurrentLine;
    bool bCommitRedraw = false;

    //Handle Change in focus or selection.
    if (FocusMode != ObservedFocusMode || bNeedClearPrevLine)
    {
        GetDisplay().setTextSize(1);

        if (bNeedClearPrevLine)
        {
            Lines[PreviousLine].ColourFlags = EColourMode::NORMAL;
            DrawStaticLineElements(PreviousLine);
            MarkLineVarsDirty(PreviousLine);
            PreviousLine = CurrentLine;
        }

        switch(FocusMode)
        {
            case EFocusMode::NoFocus:
                //no focus - no highlights
                Lines[CurrentLine].ColourFlags = EColourMode::NORMAL;
                break;
            case EFocusMode::PageFocus:
                //page focus - highlight lines.
                Lines[CurrentLine].ColourFlags = EColourMode::INVERT;
                break;
            case EFocusMode::ItemFocus:
                 //item focus - item only
                Lines[CurrentLine].ColourFlags = EColourMode::INVERT_VARS;
                break;
        }

        DrawStaticLineElements(CurrentLine);
        MarkLineVarsDirty(CurrentLine);
        ObservedFocusMode = FocusMode;
        bCommitRedraw = true;
    }

    bCommitRedraw |= DrawVariables();
    if (bCommitRedraw)
    {
        GetDisplay().display();
    }
}

void InteractivePage::Update(Keypad& Keys)
{
    if (IsPageFocused())
    {
        if (Keys.WasPressedEat(EPadButton::Up))
        {
            if (IsItemFocused())
            {
                EditValueUp();
            }
            else
            {
                ScrollUp();
            }         
        }
        
        if (Keys.WasPressedEat(EPadButton::Down))
        {
            if (IsItemFocused())
            {
                EditValueDown();
            }
            else
            {
                ScrollDown();
            }
        }
        
        if (Keys.WasPressedEat(EPadButton::Right))
        {
            if (Lines[CurrentLine].PageLink != UINT8_MAX)
            {
                PageManager.PushPage(PagesMemory[Lines[CurrentLine].PageLink]);
            }

            if (Lines[CurrentLine].VarEditIdx != UINT8_MAX)
            {
                SetFocusMode(EFocusMode::ItemFocus);
            }
        }
    }
}*/