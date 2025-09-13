#include "pages/screen_dynamic_data.h"

PageVariableAllocation PageInt::Uninitialised;

PreAllocatedTables& GetAllocationsTable()
{
    static PreAllocatedTables AllocationsTable;
    return AllocationsTable;
}

uint8_t PreAllocatedTables::AddCallback(const ActionCbk& Cbk)
{
    if (CallbackCount < CallbacksMax)
    {
        CallbackTable[CallbackCount] = Cbk;
        return ++CallbackCount;
    }
    return UINT8_MAX;
}

void PreAllocatedTables::InvokeCallback(uint8_t Idx)
{
    if(Idx < CallbackCount)
    {
        CallbackTable[Idx]();
    }
}