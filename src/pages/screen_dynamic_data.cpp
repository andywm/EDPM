#include "pages/screen_dynamic_data.h"

PageVariableAllocation PageInt::Uninitialised;

PreAllocatedTables& GetAllocationsTable()
{
    static PreAllocatedTables AllocationsTable;
    return AllocationsTable;
}

uint8_t PreAllocatedTables::AddCallback(const ActionCbk& Cbk)
{
    uint8_t InsertIndex = CallbackCount;
    if(InsertIndex < CallbacksMax)
    {
        CallbackCount++;
        CallbackTable[InsertIndex] = Cbk;
        return InsertIndex;
    }
    return UINT8_MAX;
}

void PreAllocatedTables::InvokeCallback(uint8_t Idx)
{
    if (Idx < CallbackCount)
    {
        CallbackTable[Idx]();
    }
}