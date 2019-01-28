/**
* @file  conf_events_framework.h
* @brief Configuracion del framework de eventos
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _CONF_EVENTS_FRAMEWORK_H_
#define _CONF_EVENTS_FRAMEWORK_H_

#include "events_framework.h"

/**
* @def EF_PRIORITY_MAX
* @brief Prioridad maxima utilizada por el framework de eventos
* @note Definir debajo la maxima prioridad respetando una de las siguientes:
* LOW_PRIORITY
* MEDIUM_PRIORITY
* HIGH_PRIORITY
*/
#define EF_PRIORITY_MAX          LOW_PRIORITY

/**
* @def EF_QUEUE_ELEMENTS_MAX
* @brief Cantidad maxima de elementos de las colas de eventos
* @note Cambiar segun la aplicacion
*/

#define EF_QUEUE_ELEMENTS_MAX    15

/* Declare below application modules. Those must be defined outside of this module. */

extern void ledsModuleHandler(event_t *event);
extern void buttonsModuleHandler(event_t *event);
extern void reportModuleHandler(event_t *event);


/**
* @def MODULES_STUFF
* @brief Definicion de los modulos y su prioridad
*/
#define MODULES_STUFF {                                           \
                        {  .eventHandler = ledsModuleHandler,     \
                           .priority = LOW_PRIORITY               \
                        },                                        \
                        {                                         \
                           .eventHandler = buttonsModuleHandler,  \
                           .priority = LOW_PRIORITY               \
                        },                                        \
                        {  .eventHandler = reportModuleHandler,   \
                           .priority = LOW_PRIORITY               \
                        }                                         \
                       };

/* Configure below the timer service */

/* SOFTWARE TIMER CONFIGURATIONS */
/**
* @def TIMER_SW_MAX
* @brief Maxima cantidad de timers de software soportados
* @note Cambiar segun la aplicacion
*/
#define TIMER_SW_MAX    10

/* HARDWARE TIMER CONFICURATIONS */

/**
* @def TIMER_HW
* @brief Timer a utilizar por la aplicacion
*/
#define TIMER_HW                       LPC_TIMER0

/**
* @def TIMER_HW_COMPARE_MATCH_NUMBER
* @brief Numero de COMPARE MATCH a utilizar por la aplicacion
*/
#define TIMER_HW_COMPARE_MATCH_NUMBER  TIMERCOMPAREMATCH0

/**
* @def RGU_TIMER_HW_RESET
* @brief RGU TIMER RESET asociado al TIMER a utilizar por la aplicacion
* @note Debe ir en concordancia con el TIMER utilizado
*/
#define RGU_TIMER_HW_RESET             RGU_TIMER0_RST

/**
* @def TIMER_HW_IRQn
* @brief Manejador de interrupciones asociado al TIMER a utilizar por la aplicacion
* @note Debe ir en concordancia con el TIMER utilizado
*/
#define TIMER_HW_IRQn                  TIMER0_IRQn

/**
* @def TIMER_HW_PRESCALER
* @brief Preescalador a utilizar por la aplicacion
* @note Seteado en 203 permite contar de a 1us.
*/
#define TIMER_HW_PRESCALER             203

#endif /* _CONF_EVENTS_FRAMEWORK_H_ */
