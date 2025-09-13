#include "pages/dpm_interact_page.h"
#include "pages/display_page_manager.h"
#include "keypad.h"
#include <Adafruit_SSD1306.h>

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
        PageVariableAllocation& Data= Allocations[Lines[CurrentLine].VarEditIdx];
        Data.bRedraw = true;
        Data.Value--;
    }
}

void InteractivePage::EditValueUp()
{
    if (CurrentLine != UINT8_MAX && Lines[CurrentLine].VarEditIdx != UINT8_MAX )
    {
        PageVariableAllocation& Data= Allocations[Lines[CurrentLine].VarEditIdx];
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
            DrawStaticLineElements(PreviousLine, Lines[PreviousLine].FunctionIdx != UINT8_MAX);
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

        DrawStaticLineElements(CurrentLine, Lines[CurrentLine].FunctionIdx != UINT8_MAX);
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

            if (Lines[CurrentLine].FunctionIdx != UINT8_MAX)
            {
                GetAllocationsTable().InvokeCallback(Lines[CurrentLine].FunctionIdx);
            }
        }
    }
}