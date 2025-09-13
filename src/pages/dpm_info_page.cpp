#include "pages/dpm_info_page.h"
#include <Adafruit_SSD1306.h>

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
    for (int Variable=0; Variable<AllocatedPageVars; ++Variable)
    {
        const PageVariableAllocation& Data = Allocations[Variable];
        FormatInteger(Data.Buffer, Data.Len, Data.Value);
    }

    GetDisplay().setTextSize(1);

    for (uint8_t Line = 0; Line < 4; ++Line)
    {
        DrawStaticLineElements(Line, Lines[Line].FunctionIdx != UINT8_MAX);
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

void InfoTextPage::DrawStaticLineElements(uint8_t Line, bool bDrawBox)
{
    SetColourModeFromLine(Line, EColourMode::INVERT);
    GetDisplay().setCursor(0, TextConstants::FontHeight*Line);
    size_t size = GetDisplay().println(Lines[Line].Text);

    if (bDrawBox)
    {
         GetDisplay().drawRect(0, 
            TextConstants::FontHeight*Line,
            TextConstants::FontWidth * size, 
            TextConstants::FontHeight, 
            GetFGColour());
    }
}

void InfoTextPage::MarkLineVarsDirty(uint8_t Line)
{
    for (int Variable = 0; Variable < AllocatedPageVars; ++Variable)
    {
        PageVariableAllocation& Data = Allocations[Variable];
        if (Data.Line == Line)
        {
            Data.bRedraw = true;
        }
    }
}

bool InfoTextPage::DrawVariables()
{
    bool bCommitRedraw = false;

    for (int Variable = 0; Variable < AllocatedPageVars; ++Variable)
    {
        const PageVariableAllocation& Data = Allocations[Variable];
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