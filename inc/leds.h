/**
* @file  leds.h
* @brief Manejo de los leds
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _LEDS_H_
#define _LEDS_H_

/*==================[inclusiones]============================================*/

#include "sapi_datatypes.h"
#include "events_framework.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
* @fn void ledsModuleHandler(event_t *event)
* @brief Manejador de los eventos de un led
* @param event : Evento al cual reacciona el manejador
* @return Nada
*/
void ledsModuleHandler(event_t * event);

/*==================[end of file]============================================*/
#endif /* _LEDS_H_ */
