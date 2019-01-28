/**
* @file  fsm.h
* @brief Implementacion de la maquina de estados finita.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _FSM_H_
#define _FSM_H_

/*==================[inclusiones]============================================*/
#include "sapi_datatypes.h"
/*==================[tipos de datos declarados por el usuario]===============*/

typedef struct fsmState fsmState_t;

/**
* @def void (*fsmActionCb_t) (const void * arg)
* @brief Callback de accion asociado a un estado de la maquina de estados finita
*/
typedef void (*fsmActionCb_t) (void * arg);

/**
* @struct fsmState
* @brief Formato de un estado de la maquina de estados finita */
struct fsmState {
   const uint8_t event;						/**< Evento asociado al estado que dispara la transicion */
   const fsmState_t* const nextState;  /**< Puntero al proximo estado */
   const fsmActionCb_t actionCb;			/**< Callback de accion a ejecutar en la transicion */
};

/*==================[macros]=================================================*/

/**
* @def DEFINE_FSM_STATE(name,...)
* @brief Macro para definir la tabla de estados finita
* @example #define DEFINE_FSM_STATE(Estado0,
                          {EVENTO_1, Estado1, accionTipo1}
                          ,{EVENTO_2, Estado2, accionTipo2}
                          ,{EVENTO_3, Estado3, accionTipo3}
                         )
* @note Los campos EVENTO_n no pueden valer 0 (vease fsm.h)
*/
#define DEFINE_FSM_STATE(name,...)                 \
                                                   \
static const fsmState_t name[] = {                 \
                                      __VA_ARGS__  \
                                      ,  {0,0,0}   \
                                   };

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
 * @brief Funcion generica que permite avanzar la maquina de estados finita
 	implementada por algun modulo que conforma el programa.
    La misma debe ser invocada desde el superloop principal del programa
 * @param state 		: Direccion del puntero al estado actual de la maquina de estados finita
 * @param currentEvent  : Evento actual del modulo invocante. Su valor debe ser distinto de 0.
 * @param arg 			:Argumento que sera pasado por parametro al callback de accian
						 asociado con el estado actual de la maquina de estados finita
 * @return Nada
 */
void fsm(fsmState_t** state, const uint8_t currentEvent, void * arg);

/*==================[end of file]============================================*/
#endif /* _FSM_H_ */
