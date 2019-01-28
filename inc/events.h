/**
* @file  events.h
* @brief Manejo de eventos.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _EVENTS_H_
#define _EVENTS_H_

/*==================[inclusiones]============================================*/
#include "sapi_datatypes.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/**
* @struct event_t
* @brief Estructura de un evento
*/
typedef struct {
	void * 	receptor;			/**< Modulo receptor del evento */
	uint32_t signal; 			/**< Señal a enviar al receptor */
	uint32_t value;				/**< Valor del evento */
} event_t;

/*==================[declaraciones de datos internos]========================*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/**
* @fn void events_dispatcherTask(void *paramEventQueue)
* @brief Tarea que despacha eventos de la cola que recibe por parametros
* @param paramEventQueue : Cola de eventos sobre la cual despachar los eventos 
* @return Nada
*/
void events_dispatcherTask(void *paramEventQueue);


/**
* @fn void events_pushEvent(void *receptor, uint32_t signal, uint32_t value)
* @brief Agregado de un evento en la cola de eventos
* @param receptor : Modulo receptor del evento
* @param signal   : Señal a enviar al evento
* @param value    : Valor del evento
* @return Nada
*/
void events_pushEvent(void *receptor, uint32_t signal, uint32_t value);

/**
* @fn void events_pushEventFromISR(void *receptor, uint32_t signal, uint32_t value)
* @brief Agregado de un evento en la cola de eventos
* @param receptor : Modulo receptor del evento
* @param signal   : Señal a enviar al evento
* @param value    : Valor del evento
* @return Flag de cambio de contexto
* @note UTILIZAR EN CONTEXTO DE ISR.
*/
long events_pushEventFromISR(void *receptor, uint32_t signal, uint32_t value);

/*==================[end of file]============================================*/


#endif /* _EVENTS_H_ */
