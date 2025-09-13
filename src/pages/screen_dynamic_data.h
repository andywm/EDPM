#pragma once
#include <stdint.h>
#include <functional>

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
};

struct PageVariableAllocation //VariableDescriptor
{
    char* Buffer = nullptr; //16 - Text Buffer
    int Value = 0;			//16 - Underlying Value
    uint8_t EditIdx = 0xff;	//8 - Optional index into edit config
    uint8_t Len = 0;		//8 - Length of value
    uint8_t Line = 0;		//8 - Line of value
    uint8_t Cursor = 0;		//8 - position in buffer of value.
    bool bRedraw = false;	//8 - control flag to request a redraw 
};

/**
 * Integer type for page dynamic elements. Note this is a proxy type,
 * the actual allocation lives inside the page. This type will automatically
 * be bound if registered using one of the variable functions.
 */
struct PageInt
{
    static PageVariableAllocation Uninitialised;
    PageVariableAllocation* Data = &Uninitialised;

    PageInt() = default;
    PageInt(const PageInt&) = delete; //nocopy.

    void OnRegistered(PageVariableAllocation& Mem)
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

using ActionCbk = std::function<void(void)>;
class PreAllocatedTables
{
public:
	constexpr static uint8_t CallbacksMax = 10; 

	uint8_t AddCallback(const ActionCbk& Cbk);
	void InvokeCallback(uint8_t Idx);

//private:
	ActionCbk CallbackTable[CallbacksMax] = {nullptr};
	uint8_t CallbackCount = 0;
};

extern PreAllocatedTables& GetAllocationsTable();