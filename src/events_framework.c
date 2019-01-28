/*============================================================================
 * Autores: Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo 
 * Licencia: 
 * Fecha: 17/10/2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/
#include "conf_events_framework.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

/*==================[definiciones y macros]==================================*/

/*==================[declaracion de datos internos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de datos internos]=========================*/

/**
* @var const efModule_t g_efModulesArr[]
* @brief Arreglo de modulos del event framework
*/
const efModule_t g_efModulesArr[] = MODULES_STUFF;

/**
* @var const uint8_t EF_MODULES_MAX 
* @brief Cantidad maxima de modulos del event framework
*/
const uint8_t EF_MODULES_MAX = (sizeof(g_efModulesArr) / sizeof(g_efModulesArr[0]));

/*==================[definiciones de datos externos]=========================*/

/* Events queues by priority */
extern xQueueHandle g_highPriorityEventsQueue;
extern xQueueHandle g_midPriorityEventsQueue;
extern xQueueHandle g_lowPriorityEventsQueue;

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/
bool_t eventsFramework_init() {

#if (2 /*HIGH_PRIORITY*/ <= EF_PRIORITY_MAX) /**El valor deberia ser la definicion pero no funciona 
                                                como se espera.
                                                El valor de HIGH_PRIORITY es 2
                                                */ 

    /* Create queue to handle high priority events */
    g_highPriorityEventsQueue = xQueueCreate(EF_QUEUE_ELEMENTS_MAX, sizeof(event_t));

    if(NULL == g_highPriorityEventsQueue)
       return FALSE;

    /* Create task to process to high priority events */
    if (pdPASS != xTaskCreate(events_dispatcherTask,              // Task Function
                             "highPrioEventsDispatcherTask",      // Task Name as string for user
                             2 * configMINIMAL_STACK_SIZE,        // Task Stack size
                             (void *) g_highPriorityEventsQueue,  // Task Parameters
                             tskIDLE_PRIORITY + 3,                // Task Priority
                             NULL                                 // Task Handler
                            ))
      return FALSE;
#endif

#if (1 /*MID_PRIORITY*/ <= EF_PRIORITY_MAX)

    /* Create queue to handle mid priority events */
    g_midPriorityEventsQueue = xQueueCreate(EF_QUEUE_ELEMENTS_MAX, sizeof(event_t));

    if(NULL == g_midPriorityEventsQueue)
       return FALSE;

    /* Create task to process to mid priority events */
    if (pdPASS != xTaskCreate(events_dispatcherTask,              // Task Function
                             "midPrioEventsDispatcherTask",       // Task Name as string for user
                             2 * configMINIMAL_STACK_SIZE,        // Task Stack size
                             (void *) g_midPriorityEventsQueue,   // Task Parameters
                             tskIDLE_PRIORITY + 2,                // Task Priority
                             NULL                                 // Task Handler
                            ))
     	return FALSE;
#endif

#if (0 /*LOW_PRIORITY*/ <= EF_PRIORITY_MAX)

    /* Create queue to handle low priority events */
    g_lowPriorityEventsQueue = xQueueCreate(EF_QUEUE_ELEMENTS_MAX, sizeof(event_t));

    if(NULL == g_lowPriorityEventsQueue)
       return FALSE;

    /* Create task to process to low priority events */
    if (pdPASS != xTaskCreate(events_dispatcherTask,              // Task Function
                             "lowPrioEventsDispatcherTask",       // Task Name as string for user
                             2 * configMINIMAL_STACK_SIZE,        // Task Stack size
                             (void *) g_lowPriorityEventsQueue,   // Task Parameters
                             tskIDLE_PRIORITY + 1,                // Task Priority
                             NULL                                 // Task Handler
                            ))
      return FALSE;
#endif

    /* Initialize application EF module instances */
    for(uint8_t moduleIndex = 0; moduleIndex < EF_MODULES_MAX; moduleIndex++)
       events_pushEvent((void *) &g_efModulesArr[moduleIndex], SIGNAL_INIT, (uint32_t) &g_efModulesArr[moduleIndex]);

    return TRUE;    
}


/*==================[fin del archivo]========================================*/

