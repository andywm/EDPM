#include "pages/dpm_base_page.h"
#include "pages/display_page_manager.h"

void ScreenPage::SetFocusMode(EFocusMode NewFocusMode)
{
    if (bEnableFocusFeatures)
    {
        FocusMode = NewFocusMode;
    }
}

bool ScreenPage::IsPageFocused() const
{
    //item implies page.
    return FocusMode > EFocusMode::NoFocus;
}

bool ScreenPage::IsItemFocused() const
{
    return FocusMode == EFocusMode::ItemFocus;
}

Adafruit_SSD1306& ScreenPage::GetDisplay()
{
    return *PageManager.Display;
}