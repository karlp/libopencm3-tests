#pragma once

/* Define configASSERT() to call vAssertCalled() if the assertion fails.  The assertion
has failed if the value of the parameter passed into configASSERT() equals zero. */
void vAssertCalled( const char * const pcFileName, unsigned long ulLine );
#define configASSERT(x)     if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( ( unsigned long ) 32e6 )
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE                128 /* this is in _WORDS_ */
#define configTOTAL_HEAP_SIZE                   (5 * 1024)
#define configMAX_TASK_NAME_LEN		20
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		0
#define configUSE_CO_ROUTINES 		0


#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE


#define configMAX_PRIORITIES		( 4 )
#define configMAX_CO_ROUTINE_PRIORITIES ( 4 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		0
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				0
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			0
#define INCLUDE_vTaskDelayUntil			0
#define INCLUDE_vTaskDelay				1

#define configKERNEL_INTERRUPT_PRIORITY 		0xff
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	(5 << 4)

#define vPortSVCHandler sv_call_handler
#define xPortPendSVHandler pend_sv_handler
#define xPortSysTickHandler sys_tick_handler

