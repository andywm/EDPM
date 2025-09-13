
#include <Arduino.h>
#include <Wire.h>
#include "intrusive_list.h"
#include "keypad.h"

/*

class Adafruit_SSD1306;

struct ScreenConfig
{
	ScreenConfig();
	uint8_t Flags; //SSD1306_SWITCHCAPVCC
	uint8_t I2CAddress;// = 0x3C; 
};

extern void InitialiseDisplaySubsystem(int Width, int Height, TwoWire& I2CBus, ScreenConfig Config = ScreenConfig());

struct TextConstants
{
	constexpr static int HChars = 20; 
	constexpr static int VChars = 4; 
	constexpr static int HCharsBufferLength = HChars + 1; 
	constexpr static int FontHeight = 10; //it's actually 8, but I want line spacing. 
	constexpr static int FontWidth = 6; 
};

struct Keypad;

enum class EFocusMode : uint8_t
{
	NoFocus,
	PageFocus,
	ItemFocus
};

enum class EColourMode : uint8_t
{
    NORMAL = 0,
    INVERT_VARS = 1,
    INVERT = 2,
};

enum class EPayloadType : uint8_t
{
    NONE = 0,
    PAGE_LINK = 1,
    VARIABLE = 1,
    FUNCTION = 2,
};*/

/**
 * Display page base
 */
/*
struct ScreenPage : IntrusiveDoublyLinkedList<ScreenPage>
{
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void Update(Keypad& Keys) {};
	virtual void Draw() = 0;

	EFocusMode FocusMode = EFocusMode::NoFocus;
	bool bEnableFocusFeatures = false;

	void SetFocusMode(EFocusMode NewFocusMode)
	{
		if (bEnableFocusFeatures)
		{
			FocusMode = NewFocusMode;
		}
	}

	bool IsPageFocused() const
	{
		//item implies page.
		return FocusMode > EFocusMode::NoFocus;
	}

	bool IsItemFocused() const
	{
		return FocusMode == EFocusMode::ItemFocus;
	}

    Adafruit_SSD1306& GetDisplay();
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

struct HasFocusAttribute
{
	constexpr HasFocusAttribute(ScreenPage& Page)
	{
		Page.bEnableFocusFeatures = true;
	}
};

struct ScreenPageManager
{
	ScreenPage* LinkedList = nullptr;
	ScreenPage* CurrentPage = nullptr;
	ScreenPage* PageRequest = nullptr;

	ScreenPage& RegisterRoot(ScreenPage& Page)
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

	void Update(Keypad& keypad)
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

	void Next()
	{
		if (LinkedList)
		{
			LinkedList = LinkedList->Next();
			PushPage(LinkedList, false);
		}
	}

	void Previous()
	{
		if (LinkedList)
		{
			LinkedList = LinkedList->Prev();
			PushPage(LinkedList, false);
		}
	}

	void PushPage(ScreenPage* Page, bool bAutoFocus = true)
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

	void PopPage()
	{
		if (CurrentPage->ForwardNode == nullptr)
		{
			PageRequest = CurrentPage->BackwardNode;
			PageRequest->SetFocusMode(EFocusMode::PageFocus);
			return;
		}
	}

	void SelectPage()
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

	ScreenPage* Current() { return CurrentPage;}
};*/

//extern ScreenPageManager PageManager;

/**
 * Text Page Type
 */
/*
struct InfoTextPage : public ScreenPage
{	
	struct Line
	{
		char Text[TextConstants::HCharsBufferLength] = "";
		EColourMode ColourFlags = EColourMode::NORMAL;
		uint8_t PageLink = 255;
		uint8_t VarEditIdx = 255;
		uint8_t FunctionIdx = 255;
	};
	
	struct DebugData
	{
		char* Buffer = nullptr; //16 - Text Buffer
		int Value = 0;			//16 - Underlying Value
		uint8_t EditIdx = 0xff;	//8 - Optional index into edit config
		uint8_t Len = 0;		//8 - Length of value
		uint8_t Line = 0;		//8 - Line of value
		uint8_t Cursor = 0;		//8 - position in buffer of value.
		bool bRedraw = false;	//8 - control flag to request a redraw 
	};

	struct IntegerProxy
	{
		static DebugData NotSet;
		DebugData* Data = &NotSet;

		void OnRegistered(DebugData& Mem)
		{
			Data = &Mem;
		}

		int operator=(int Val)
		{
			Data->bRedraw = true; //Data->Value != Val;
			Data->Value = Val;
			return Val;
		}

		int operator+=(int Val)
		{
			Data->bRedraw = true; //Data->Value != Val;
			Data->Value += Val;
			return Data->Value;
		}

		int operator-=(int Val)
		{
			Data->bRedraw = true; //Data->Value != Val;
			Data->Value -= Val;
			return Data->Value;
		}
	};

	struct CallbackProxyBase : public IntrusiveDoublyLinkedList<CallbackProxyBase>
	{
		virtual void Invoke() = 0;
	};

	template<typename T>
	struct CallbackProxy final : public CallbackProxyBase 
	{
		using CbkType = void(T::*)();

		T* Target = nullptr;
		CbkType Function = nullptr;

		template<CbkType InFn>
		CallbackProxy(T& InTarget) : Target(&InTarget), Function(InFn) {}

		virtual void Invoke() override 
		{
			(Target)->*Function();
		}
	};

	template<typename T, uint8_t Num>
	struct CallbackArray
	{
		CallbackProxy<T> Callback;
	};


	Line Lines[TextConstants::VChars];
	DebugData Allocations[10];
	uint8_t AllocatedDebugVars = 0;
	uint8_t MaxLine = 0;

	template<uint8_t LineNum, typename ...TArgs>
	void SetLine(const char* Text, TArgs&&... Args)
	{
		if (LineNum >= MaxLine)
		{
			MaxLine = LineNum;
		}

		strncpy(Lines[LineNum].Text, Text, min(TextConstants::HChars, strlen(Text)) + 1);
		InjectUserVariables(Lines[LineNum].Text, LineNum, 0, Args...);
	}
	
	template<typename ...TArgs>
	void InjectUserVariables(char* Text, uint8_t LineIdx, uint8_t CharIdx, IntegerProxy& DVar, TArgs&&... Args)
	{
		for (; CharIdx < TextConstants::HCharsBufferLength; ++CharIdx)
		{
			if (Text[CharIdx] == '#')
			{
				const int Inner = CharIdx;
				int Outer = CharIdx;

				while (CharIdx < TextConstants::HChars && Text[CharIdx] == '#')
				{
					Text[CharIdx] = '_';
					Outer = CharIdx++;
				}
				
				DebugData& Page = Allocations[AllocatedDebugVars++];
				Page.Buffer = &Text[Inner];
				Page.Len = (Outer - Inner) + 1;
				Page.Line = LineIdx;
				Page.Cursor = Inner;

				DVar.OnRegistered(Page);

				//next var
				InjectUserVariables(Text, LineIdx, CharIdx, Args...);
			}
		}
	}
	void InjectUserVariables(const char* Text, uint8_t LineIdx, uint8_t CharIdx){}

	virtual int GetBGColour();
	virtual int GetFGColour();

    static void FormatInteger(char* Buffer, int BufferLength, int16_t Value);

    virtual void Activate() override;
	virtual void Deactivate() override;

	void SetColourModeFromLine(uint8_t Line, EColourMode Threshold);
	void DrawStaticLineElements(uint8_t Line);
	void MarkLineVarsDirty(uint8_t Line);
	bool DrawVariables();

	virtual void Draw() override;
};
*/
/*
struct InteractivePage : public InfoTextPage
{
	HasFocusAttribute EnableFocus{*this};
	CallbackProxyBase* ActionList =  nullptr;
	EFocusMode ObservedFocusMode = EFocusMode::NoFocus;
	uint8_t PreviousLine = 255;
	uint8_t CurrentLine = 0;

	template<uint8_t LineNum, uint8_t PageID, typename ...TArgs>
	void SetLineWithSubpage(const char* Text, TArgs&&... Args)
	{
		Lines[LineNum].PageLink = PageID;
		SetLine<LineNum>(Text, std::forward<TArgs&&>(Args)...);
	}

	template<uint8_t LineNum>
	void SetLineWithVar(const char* Text, IntegerProxy& Var)
	{
		SetLine<LineNum>(Text, Var);
		Lines[LineNum].VarEditIdx = AllocatedDebugVars - 1; 
	}

	template<uint8_t LineNum, typename T>
	void SetLineAsButton(const char* Text, CallbackProxy<T>* Action)
	{
		SetLine<LineNum>(Text);
		if (ActionList)
		{
			ActionList->AddAsRootSet(Action);
		}
		else
		{
			ActionList = Action;
		}
	}

	void ScrollDown();
	void ScrollUp();
	void EditValueDown();
	void EditValueUp();

    virtual void Activate() override;
	virtual void Deactivate() override;
	virtual void Draw() override;
	virtual void Update(Keypad& Keys) override;
};

struct GraphicsPage : public ScreenPage
{
	
};
*/