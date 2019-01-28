/** 
* @file  timer.c
* @brief Manejo de timer del lpc4337.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
    Licencia: 
    Fecha: 17/10/2018
*/

/*==================[inclusions]=============================================*/
#include "timer_service.h"
#include "conf_events_framework.h"

/* For UINT32_MAX */
#include <stdint.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/*==================[macros and definitions]=================================*/
/**
* @def OVERFLOW_TICKS
* @brief Ticks necesarios para que el timer haga overflow
*/
#define OVERFLOW_TICKS  UINT32_MAX

/**

* @struct timerHwData_t
* @brief Datos de configuracion del timer de hardware
*/
typedef struct {

  LPC_TIMER_T *name;
  timerCompareMatch_t compareMatchNumber;
  uint32_t RGU;                                 /* Reset Generator Unit */
  uint32_t IRQn;
  SemaphoreHandle_t timerMutex;
} timerHwData_t;

/**
* @struct timerSwData_t
* @brief Datos de configuracion del timer de software
*/
typedef struct {
   void *      receptor;
   uint32_t    period;
   uint32_t    timeout;
   bool_t      isOneShot;
   bool_t      isEnabled;
} timerSwData_t;

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/**
* @var static bool_t g_initFlag
* @brief Indicador del estado de inicializacion del driver
*/
static bool_t g_initFlag = FALSE;

/**
* @var static volatile uint64_t g_timerOverflowCount
* @brief Variable contador de overflow del timer
*/
static volatile uint64_t g_timerOverflowCount;

/**
* @var static timerHwData_t g_timerHw
* @brief Data del timer de hardware
* @note Data sacada de las librerias del uC
*/
static timerHwData_t g_timerHw = {
   .name = TIMER_HW
,  .compareMatchNumber = TIMER_HW_COMPARE_MATCH_NUMBER
,  .RGU = RGU_TIMER_HW_RESET
,  .IRQn = TIMER_HW_IRQn
,  .timerMutex = NULL
};

/**
* @var static timerSwData_t g_timerSwArr
* @brief Arreglo de timers de software
*/
static timerSwData_t g_timerSwArr[TIMER_SW_MAX];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[ISR external functions definition]======================*/
/**
* @fn void TIMER0_IRQHandler(void)
* @brief Manejador de interrupcion del timer0
* @param Ninguno
* @return Nada
* @note  Handler for ISR TIMER0
*/
void TIMER0_IRQHandler(void){

  if(Chip_TIMER_MatchPending(g_timerHw.name, g_timerHw.compareMatchNumber)) {

    g_timerOverflowCount++;
    Chip_TIMER_ClearMatch(g_timerHw.name, g_timerHw.compareMatchNumber);
  }
}

/**
Ver -->  https://www.freertos.org/a00016.html

Tick Hook Function
The tick interrupt can optionally call an application defined hook (or callback) function - the tick hook.
The tick hook provides a convenient place to implement timer functionality.
The tick hook will only get called if configUSE_TICK_HOOK is set to 1 within FreeRTOSConfig.h.
When this is set the application must provide the hook function with the following prototype:

void vApplicationTickHook( void );
vApplicationTickHook() executes from within an ISR so must be very short, not use much stack,
and not call any API functions that don't end in "FromISR" or "FROM_ISR".

**/

void vApplicationTickHook(void)  {

   portBASE_TYPE contextSwitch = pdFALSE;

   for(uint32_t i = 0; TIMER_SW_MAX > i; i++)   {

      if(TRUE == g_timerSwArr[i].isEnabled)  {

         if(g_timerSwArr[i].timeout && ((--g_timerSwArr[i].timeout) == 0)) {

            contextSwitch = events_pushEventFromISR(g_timerSwArr[i].receptor, SIGNAL_TIMEOUT, 0);

            if(g_timerSwArr[i].isOneShot) {
               g_timerSwArr[i].isEnabled = FALSE;
            }  else  {
               g_timerSwArr[i].timeout = g_timerSwArr[i].period;
            }
         }
      }
   }

   portEND_SWITCHING_ISR(contextSwitch);
}

/*==================[external functions definition]==========================*/

bool_t timerService_initHw(void) {

   /* Create timer accessing mutex semaphore */
   g_timerHw.timerMutex = xSemaphoreCreateMutex();
  
   if(NULL != g_timerHw.timerMutex) {

      /* Initialize overflow variable counter */
      g_timerOverflowCount = 0;

      /* Enable timer clock and reset it */
      Chip_TIMER_Init(g_timerHw.name);

      Chip_RGU_TriggerReset(g_timerHw.RGU);
      while (Chip_RGU_InReset(g_timerHw.RGU))
      ;

      Chip_TIMER_Reset(g_timerHw.name);

      Chip_TIMER_PrescaleSet(g_timerHw.name, TIMER_HW_PRESCALER);

      /* Initialize compare match with the specified ticks (number of counts needed to clear the match counter) */

      Chip_TIMER_MatchEnableInt(g_timerHw.name, g_timerHw.compareMatchNumber);
      Chip_TIMER_SetMatch(g_timerHw.name, g_timerHw.compareMatchNumber, OVERFLOW_TICKS);

      /* Para mi se para el contador si usamos reset */
      //Chip_TIMER_ResetOnMatchEnable( timer[timerNumber].name, compareMatchNumber);
      /* Enable timer */
      Chip_TIMER_Enable(g_timerHw.name);

      /* Enable timer */
      Chip_TIMER_Enable(g_timerHw.name);

      /* Enable timer interrupt */
      NVIC_EnableIRQ(g_timerHw.IRQn);
      NVIC_ClearPendingIRQ(g_timerHw.IRQn);

      /* Flag driver is initialized */
      g_initFlag = TRUE;

      return TRUE;
   }

   return FALSE;
}

bool_t timerService_readHwCounterValue(uint64_t *timeUs) {

   if((FALSE != g_initFlag) && (NULL != timeUs)) {

      /* Try take mutex semaphore */
      if(pdPASS == xSemaphoreTake(g_timerHw.timerMutex , 0)) {

         uint64_t _timerOverflowCount;
         uint32_t _timerTickCount;

         /* Busy wait sentence to ensure timer data is correct */
         do {

            /* Read g_timerOverflowCount */
            _timerOverflowCount = g_timerOverflowCount;
            _timerTickCount = Chip_TIMER_ReadCount(g_timerHw.name);
         } while(_timerOverflowCount != g_timerOverflowCount);

         /* Complete timer value to be returned */
         *timeUs = (_timerOverflowCount*((uint64_t) OVERFLOW_TICKS)) + (uint64_t) _timerTickCount;

         /* Give mutex */
         xSemaphoreGive(g_timerHw.timerMutex);

         return TRUE;
      }
   }

   return FALSE;
}

bool_t timerService_createSwTimerOneShot(void *receptor, uint32_t timeoutMs) {

   bool_t ret = FALSE;

   portENTER_CRITICAL();

   /* Find empty timer slot */
   for(uint32_t i = 0;TIMER_SW_MAX > i;i++) {

      if(FALSE == g_timerSwArr[i].isEnabled) {
         /* Feel empty slot and start timer */
         g_timerSwArr[i].receptor = receptor;
         g_timerSwArr[i].period = 0;
         g_timerSwArr[i].timeout = timeoutMs;
         g_timerSwArr[i].isOneShot = TRUE;
         g_timerSwArr[i].isEnabled = TRUE;

         ret = TRUE;
         break;
      }
   }

   portEXIT_CRITICAL();

   return ret;
}

bool_t timerService_createSwTimerPeriodic(void *receptor, uint32_t timeoutMs) {

   bool_t ret = FALSE;

   portENTER_CRITICAL();

   /* Find empty timer slot */
   for(uint32_t i = 0;TIMER_SW_MAX > i;i++) {

      if(FALSE == g_timerSwArr[i].isEnabled) {
         /* Feel empty slot and start timer */
         g_timerSwArr[i].receptor = receptor;
         g_timerSwArr[i].period = timeoutMs;
         g_timerSwArr[i].timeout = timeoutMs;
         g_timerSwArr[i].isOneShot = FALSE;
         g_timerSwArr[i].isEnabled = TRUE;

         ret = TRUE;
         break;
      }
   }

   portEXIT_CRITICAL();

   return ret;
}

void timerService_destroySwTimer(void *receptor) {
   portENTER_CRITICAL();

   /* Find desired timer slot */
   for(uint32_t i = 0;TIMER_SW_MAX > i;i++) {

      if(receptor == g_timerSwArr[i].receptor) {
         /* Stop timer */
         g_timerSwArr[i].period = 0;
         g_timerSwArr[i].timeout = 0;
         g_timerSwArr[i].isEnabled = FALSE;
         break;
      }
   }

   portEXIT_CRITICAL();
}

/*==================[end of file]============================================*/
