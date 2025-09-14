#pragma once

#include "pages/dpm_info_page.h"
#include "pages/screen_dynamic_data.h"

struct InteractivePage : public InfoTextPage
{
	HasFocusAttribute EnableFocus{*this};
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
	void SetLineWithVar(const char* Text, PageInt& Var)
	{
		SetLine<LineNum>(Text, Var);
		Lines[LineNum].VarEditIdx = AllocatedPageVars - 1; 
	}

	template<uint8_t LineNum>
	void SetLineAsButton(const char* Text, const ActionCbk& Callback)
	{
		SetLine<LineNum>(Text);
		Lines[LineNum].FunctionIdx = GetAllocationsTable().AddCallback(Callback);	
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