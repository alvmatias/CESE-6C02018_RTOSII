/**
* @file  events_framework.h
* @brief Manejo del framework de eventos.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _EVENTS_FRAMEWORK_H_
#define _EVENTS_FRAMEWORK_H_

/*==================[inclusiones]============================================*/

/* Events framework related files */
#include "sapi_datatypes.h"
#include "events.h"
#include "seniales.h"
#include "timer_service.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/
/**
* @enum efPriority_t
* @brief Prioridades de un evento
*/
typedef enum {
   LOW_PRIORITY     = 0x00		/**< Prioridad Baja */
,  MID_PRIORITY					/**< Prioridad Media */
,  HIGH_PRIORITY				/**< Prioridad Alta */
} efPriority_t;


/**
* @def void (*efEventHandler_cb) (event_t *)
* @brief Definicion de manejador de evento
*/
typedef void (*efEventHandler_cb) (event_t *);

/**
* @struct efModule_t
* @brief Estructura de un Modulo
*/
typedef struct {
   efEventHandler_cb    eventHandler; 		/**< Manejador de evento */
   efPriority_t         priority;			/**< Prioridad */
} efModule_t;

/*==================[declaraciones de datos internos]========================*/

/*==================[declaraciones de datos externos]========================*/
extern const efModule_t g_efModulesArr[];

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/**
* @fn bool_t eventsFramework_init()
* @brief Inicializacion del framework de eventos
* @param Ninguno
* @return bool_t TRUE si fue correcta la inicializacion, FALSE caso contrario
*/
bool_t eventsFramework_init();

/*==================[end of file]============================================*/

#endif /* _EVENTS_FRAMEWORK_H_ */
