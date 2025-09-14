#pragma once

#include "page_list.h"
#include "keypad.h"

class Adafruit_SSD1306;

enum class EFocusMode : uint8_t
{
	NoFocus,
	PageFocus,
	ItemFocus
};

/**
 * Display page base
 */
struct ScreenPage : IntrusiveDoublyLinkedList<ScreenPage>
{
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void Update(Keypad& Keys) {};
	virtual void Draw() = 0;

	EFocusMode FocusMode = EFocusMode::NoFocus;
	bool bEnableFocusFeatures = false;

	void SetFocusMode(EFocusMode NewFocusMode);
	bool IsPageFocused() const;
	bool IsItemFocused() const;

    Adafruit_SSD1306& GetDisplay();
};

struct HasFocusAttribute
{
	constexpr HasFocusAttribute(ScreenPage& Page)
	{
		Page.bEnableFocusFeatures = true;
	}
};

template<uint8_t Count>
struct ChildPageArray
{
	ScreenPage* Array[Count];

	ChildPageArray(ScreenPage& Self)
	{
		Self.ChildPagesCount = Count;
		Self.PagesMemory = &Array[0];
	}
};
