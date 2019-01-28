/** 
* @file  to_minus_command.c
* @brief Implementacion del procesador del comando "To Minus"
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
    Licencia: 
    Fecha: 17/10/2018
*/

/*==================[inlcusiones]============================================*/
#include <string.h>
#include "to_minus_command.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/*==================[definiciones y macros]==================================*/


/*==================[definiciones de datos internos]=========================*/

/**
* @var static QueueHandle_t g_rxQueue_Handler 
* @brief Handler de la cola de recepcion del comando
*/
static QueueHandle_t g_rxQueue_Handler = NULL;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/
/**
* @fn static bool_t rxQueuePop(practica1Frame_t *frame)
* @brief Pop de un frame de la cola de recepcion del comando
* @param frame : Frame a obtener de la cola
* @return bool_t TRUE si se recibio correctamente, FALSE caso contrario
*/
static bool_t rxQueuePop(practica3Frame_t *frame) {

   bool_t ret = FALSE;

   if((NULL != g_rxQueue_Handler) && (NULL != frame)) {

      /* Get data to process */
      if(pdPASS == xQueueReceive(g_rxQueue_Handler, frame, portMAX_DELAY)) {

         ret = TRUE;
      }
   }

   return ret;
}

/**
* @fn static void toMinus_Task(void *params)
* @brief Tarea de ejecucion del procesador del comando "To Minus"
* @return Nada
* @note Tarea de FreeRTOS
*/
static void toMinus_Task(void *params) {

  #define TO_MINUS_CONSTANT    ('a' - 'A')
  practica3Frame_t frame;
  uint8_t *p2Data = NULL;
  uint8_t localSize = 0;

  while(TRUE) {

    if(TRUE == rxQueuePop(&frame)) {

       p2Data = frame.frameData;
       localSize = frame.frameSize;

       if((NULL != p2Data) && (0 != localSize)) {

          while(localSize) {

             uint8_t ch = *p2Data;

             if( (ch >= 'A') && (ch <= 'Z') )
                (*p2Data) = (ch + TO_MINUS_CONSTANT);

             p2Data++;
             --localSize;
          }

          frame.minimumStackSize = uxTaskGetStackHighWaterMark(NULL);   /* NULL to query its own stack */
          practica3Commands_dataPush(&frame, FALSE);
       }
    }
  }
}

/*==================[definiciones de funciones externas]=====================*/

bool_t toMinusCommand_init(void) {


   /* Create task to process to minus command */
   if (pdPASS != xTaskCreate(toMinus_Task,                        // Task Function
                             "toMinus_Task",                      // Task Name as string for user
                             2 * configMINIMAL_STACK_SIZE,        // Task Stack size
                             NULL,                                // Task Parameters
                             tskIDLE_PRIORITY + 2,                // Task Priority
                             NULL                                 // Task Handler
                            ))
      return FALSE;

   /* Create Rx queue */

   g_rxQueue_Handler = xQueueCreate(QUEUE_ENTRIES_MAX, sizeof(practica3Frame_t));

   if(NULL == g_rxQueue_Handler)
      return FALSE;

   return TRUE;
}

void toMinusCommand_dataPush(const practica3Frame_t *frame) {

  /* Push newFrame into queue */
  if((NULL != g_rxQueue_Handler) && (NULL != frame)) {

    xQueueSend(g_rxQueue_Handler, frame, 0);

  }
}

/*==================[fin del archivo]========================================*/
