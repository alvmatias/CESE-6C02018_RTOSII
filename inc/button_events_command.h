/**
* @file  button_events_command.h
* @brief Manejo del comando de reporte de evento de botones.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _BUTTON_EVENTS_COMMAND_H_
#define _BUTTON_EVENTS_COMMAND_H_

/*==================[inclusiones]============================================*/
#include "sapi_datatypes.h"
#include "events_framework.h"
/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
* @fn void reportModuleHandler(event_t *event)
* @brief Manejador del reporte de eventos de botones
* @param event : Evento al cual reacciona el manejador
* @return Nada
*/
void reportModuleHandler(event_t *event);

/*==================[end of file]============================================*/
#endif /* _BUTTON_EVENTS_COMMAND_H_ */
