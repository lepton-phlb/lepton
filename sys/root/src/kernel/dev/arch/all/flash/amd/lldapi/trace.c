
#define TRACE_C
#include "lld.h"
#include "trace.h"

extern DWORD os_GetCurrentTicks();


#define STORE_TIMESTAMP         trace[cycle].timestamp = os_GetCurrentTicks()


int polling_transitions = 0;
POLLING_TRANSITION transitions[MAX_NUM_OF_TRANSITIONS];
CYCLE trace[MAX_TRACE_CYCLES];
int cycle = -1;
int trace_wrapped = FALSE;



void StoreTraceCycle(OPERATION op, FLASHDATA * base, ADDRESS offset, ADDRESS addr, FLASHDATA data)
{
   cycle++;
   if (cycle >= MAX_TRACE_CYCLES)
   {
      cycle = 0;
      trace_wrapped = TRUE;
   }
   trace[cycle].op        = op;
   trace[cycle].base      = base;
   trace[cycle].offset    = offset;
   trace[cycle].addr      = addr;
   trace[cycle].data      = data;

}



void FlashWrite(FLASHDATA * addr, ADDRESS offset, FLASHDATA data)
{
   FLASH_OFFSET((addr),(offset)) = (data);
   StoreTraceCycle(OP_WRITE, addr, offset, (ADDRESS)((volatile FLASHDATA*)(addr) + (offset)), data);

#ifdef PRINT_TIMESTAMP
   STORE_TIMESTAMP;
#endif
}

FLASHDATA FlashRead(FLASHDATA * addr, ADDRESS offset)
{
   FLASHDATA data;
   data = FLASH_OFFSET((addr),(offset));
   StoreTraceCycle(OP_READ, addr, offset, (ADDRESS)((volatile FLASHDATA*)(addr) + (offset)), data);

#ifdef PRINT_TIMESTAMP
   STORE_TIMESTAMP;
#endif

   return(data);
}


