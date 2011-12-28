
#ifndef TRACE_H
#define TRACE_H

typedef DWORD  TIMESTAMP;

typedef enum
{
  OP_READ = 1,
  OP_WRITE
}OPERATION;

typedef struct
{
  OPERATION op;
  FLASHDATA *   base;
  ADDRESS   offset;
  ADDRESS   addr;
  FLASHDATA data;
#ifdef PRINT_TIMESTAMP
  TIMESTAMP timestamp;
#endif
}CYCLE;

typedef struct
{
    POLLING_TYPE  polling_type;    /* tells us the type of operation - erase, prog */
    FLASHDATA     polling_data[5]; /* shows us whats the last five polls looked like */
    FLASHDATA     expected_data;   /* the value being prog or erased */
    DEVSTATUS     return_code;     /* tells us what conclusion the sw had */
}POLLING_TRANSITION;

#define MAX_NUM_OF_TRANSITIONS 100
#define MAX_TRACE_CYCLES 512

#ifndef TRACE_C
extern CYCLE trace[MAX_TRACE_CYCLES];
extern int cycle;
extern int trace_wrapped;
#endif

#endif
