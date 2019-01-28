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
/**
* @var xQueueHandle g_highPriorityEventsQueue
* @brief Cola de eventos de alta prioridad
*/
xQueueHandle g_highPriorityEventsQueue = NULL;

/**
* @var xQueueHandle g_midPriorityEventsQueue
* @brief Cola de eventos de media prioridad
*/
xQueueHandle g_midPriorityEventsQueue = NULL;

/**
* @var xQueueHandle g_lowPriorityEventsQueue
* @brief Cola de eventos de baja prioridad
*/
xQueueHandle g_lowPriorityEventsQueue = NULL;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[definiciones de funciones internas]=====================*/
/**
* @fn static xQueueHandle getEventsQueue(efPriority_t priority) 
* @brief Obtencion de cola de eventos segun prioridad
* @param priority : Prioridad de la cola que se desea obtener
* @return xQueueHandle Manejador de la cola obtenida
*/
static xQueueHandle getEventsQueue(efPriority_t priority) {
   xQueueHandle evQueue = NULL;

   switch (priority)
   {
      case LOW_PRIORITY:   evQueue = g_lowPriorityEventsQueue;    break;
      case MID_PRIORITY:   evQueue = g_midPriorityEventsQueue;    break;
      case HIGH_PRIORITY:  evQueue = g_highPriorityEventsQueue;   break;
      default:
         break;
   }

   return evQueue;
}

/**
* @fn static event_t waitForEvent(xQueueHandle eventsQueue)
* @brief Espera de ocurrencia de un evento
* @param eventsQueue : Cola sobre la cual se espera el evento
* @return event_t Evento ocurrido
*/
static event_t waitForEvent(xQueueHandle eventsQueue) {
	event_t event;

	xQueueReceive(eventsQueue, &event, portMAX_DELAY);

	return event;
}

/**
* @fn static void dispatchEvent(event_t *event) 
* @brief Despacho de eventos
* @param event : Evento a despachar
* @return Nada
*/
static void dispatchEvent(event_t *event) {
	((efModule_t *) event->receptor)->eventHandler(event);
}

/*==================[definiciones de funciones externas]=====================*/

void events_dispatcherTask(void *paramEventQueue) {

	xQueueHandle eventsQueue = (xQueueHandle) paramEventQueue;
	event_t event;

	while(TRUE) {
		event = waitForEvent(eventsQueue);
		dispatchEvent(&event);
	}  
}

void events_pushEvent(void *receptor, uint32_t signal, uint32_t value) {
	xQueueHandle eventsQueue = getEventsQueue(((efModule_t *) receptor)->priority);

	if(NULL != eventsQueue) {
	   event_t event = {
                           .receptor = receptor
                        ,  .signal = signal
                        ,  .value = value
                      };

	   xQueueSend(eventsQueue, &event, 0);
	}
}

long events_pushEventFromISR(void *receptor, uint32_t signal, uint32_t value) {
   long contextSwitch = pdFALSE;
	xQueueHandle eventsQueue = getEventsQueue(((efModule_t *) receptor)->priority);

   if(NULL != eventsQueue) {
      event_t event = {
                           .receptor = receptor
                        ,  .signal = signal
                        ,  .value = value
                      };

      xQueueSendFromISR(eventsQueue, &event, &contextSwitch);
   }

	return contextSwitch;
}

/*==================[fin del archivo]========================================*/
