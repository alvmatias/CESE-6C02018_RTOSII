/**
* @file  buttons.h
* @brief Manejo de los botones
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

/*==================[inclusiones]============================================*/

#include "sapi_datatypes.h"
#include "events_framework.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/**
* @enum buttonStatus_t
* @brief Estados de un boton
*/
typedef enum {
   BUTTON_UP	=	1
,  BUTTON_DOWN
} buttonStatus_t;

/*==================[declaraciones de datos externos]========================*/
extern void * LED_MODULE_RECEPTOR;
extern void * BUTTON_MODULE_RECEPTOR;
extern void * REPORT_MODULE_RECEPTOR;

/*==================[declaraciones de funciones externas]====================*/
/**
* @fn void buttonsModuleHandler(event_t *event)
* @brief Manejador de los eventos de un boton
* @param event : Evento al cual reacciona el manejador
* @return Nada
*/
void buttonsModuleHandler(event_t *event);

/*==================[end of file]============================================*/
#endif /* _BUTTONS_H_ */
