/**
* @file  fsm.c
* @brief Implementacion de la maquina de estados finita.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

/*==================[inlcusiones]============================================*/

#include "fsm.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void fsm(fsmState_t** state, const uint8_t currentEvent, void * arg) {
   if(state) {
	   fsmState_t* tmpState = *state;

      while(tmpState->event && (currentEvent != tmpState->event))
    	  tmpState++;

      if(tmpState->event) {
    	  tmpState->actionCb(arg);
    	  *state = (fsmState_t*) tmpState->nextState;
      }
   }
}

/*==================[fin del archivo]========================================*/
