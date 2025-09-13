#pragma once

#include "pages/dpm_base_page.h"
#include "pages/screen_dynamic_data.h"
#include <cstring>
#include <algorithm> 

struct TextConstants
{
	constexpr static int HChars = 20; 
	constexpr static int VChars = 4; 
	constexpr static int HCharsBufferLength = HChars + 1; 
	constexpr static int FontHeight = 10; //it's actually 8, but I want line spacing. 
	constexpr static int FontWidth = 6; 
};

/**
 * Text Page Type
 */
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
	
	Line Lines[TextConstants::VChars];
	PageVariableAllocation Allocations[10];
	uint8_t AllocatedPageVars = 0;
	uint8_t MaxLine = 0;

	template<uint8_t LineNum, typename ...TArgs>
	void SetLine(const char* Text, TArgs&&... Args)
	{
		if (LineNum >= MaxLine)
		{
			MaxLine = LineNum;
		}

		std::strncpy(Lines[LineNum].Text, Text, std::min((size_t)TextConstants::HChars, std::strlen(Text)) + 1);
		InjectUserVariables(Lines[LineNum].Text, LineNum, 0, Args...);
	}
	
	template<typename ...TArgs>
	void InjectUserVariables(char* Text, uint8_t LineIdx, uint8_t CharIdx, PageInt& DVar, TArgs&&... Args)
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
				
				PageVariableAllocation& Page = Allocations[AllocatedPageVars++];
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
	void DrawStaticLineElements(uint8_t Line, bool bDrawBox = false);
	void MarkLineVarsDirty(uint8_t Line);
	bool DrawVariables();

	virtual void Draw() override;
};