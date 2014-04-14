#include <stdlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "partest.h"
#include "flash.h"

/* The task that is created three times. */
static portTASK_FUNCTION_PROTO( lepton_task_kernel, pvParameters );
static portTASK_FUNCTION_PROTO( lepton_task_1, pvParameters );

static xTaskHandle handle_lepton_task_kernel;
static xTaskHandle handle_lepton_task_1;

typedef struct freertos_tcb_st{
	volatile portSTACK_TYPE	*pxTopOfStack;		/*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */
}freertos_tcb_t;

freertos_tcb_t *pxTCB_task;

typedef unsigned long OS_U32;

typedef struct cpu_regs_st {
  //OS_U32  Counters;
  OS_U32  OS_REG_R4;
  OS_U32  OS_REG_R5;
  OS_U32  OS_REG_R6;
  OS_U32  OS_REG_R7;
  OS_U32  OS_REG_R8;
  OS_U32  OS_REG_R9;
  OS_U32  OS_REG_R10;
  OS_U32  OS_REG_R11;
  OS_U32  OS_REG_LR;
  OS_U32  OS_REG_R0;
  OS_U32  OS_REG_R1;
  OS_U32  OS_REG_R2;
  OS_U32  OS_REG_R3;
  OS_U32  OS_REG_R12;
  OS_U32  OS_REG_R14;
  OS_U32  OS_REG_PC;
  OS_U32  OS_REG_XPSR;
} cpu_regs_t;

typedef void (*__sa_handler)(void);
typedef __sa_handler sa_handler_t;

void sighandler(void);
sa_handler_t pfn_sighandler=sighandler;

/*--------------------------------------------
| Name:        sighandler
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void sighandler(void){
   const portTickType xDelay = 100 / portTICK_RATE_MS;
  for(;;){
     vTaskDelay( xDelay );
  }
}

/*--------------------------------------------
| Name:        lepton_start
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
void lepton_start( void ){
   /* Spawn the task. */
   xTaskCreate( lepton_task_kernel, ( signed char * ) "kernel", configMINIMAL_STACK_SIZE, NULL,  tskIDLE_PRIORITY + 10UL, ( xTaskHandle * ) &handle_lepton_task_kernel );
    /* Spawn the task. */
   xTaskCreate( lepton_task_1, ( signed char * ) "task_1", configMINIMAL_STACK_SIZE, NULL,  tskIDLE_PRIORITY + 1UL, ( xTaskHandle * ) &handle_lepton_task_1 );
}
/*--------------------------------------------
| Name:        lepton_task_kernel
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static portTASK_FUNCTION( lepton_task_kernel, pvParameters )
{
   /* Block for 1000ms. */
   const portTickType xDelay = 1000 / portTICK_RATE_MS;
   volatile cpu_regs_t* p_cpu_regs;
   
   volatile OS_U32  pc;
     
   vTaskDelay( xDelay );
   //
   vTaskSuspendAll ();
   //
   pxTCB_task = (freertos_tcb_t *)handle_lepton_task_1;
   p_cpu_regs= (cpu_regs_t*) (pxTCB_task->pxTopOfStack);
   pc = p_cpu_regs->OS_REG_PC;
   p_cpu_regs->OS_REG_PC =(OS_U32)pfn_sighandler;
   //
   xTaskResumeAll ();
   //
   for( ;; ){
       vTaskDelay( xDelay );
   }
} /*lint !e715 !e818 !e830 Function definition must be standard for task creation. */

/*--------------------------------------------
| Name:        lepton_task_kernel
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static portTASK_FUNCTION( lepton_task_1, pvParameters ){
   /* Block for 500ms. */
   const portTickType xDelay = 500 / portTICK_RATE_MS;
   //
   for( ;; ){
      vTaskDelay( xDelay );
   }
} /*lint !e715 !e818 !e830 Function definition must be standard for task creation. */

