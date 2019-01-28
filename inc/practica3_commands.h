/** 
* @file  practica3_commands.h
* @brief Definicion de los comandos de la practica 3
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia: 
		Fecha: 17/10/2018
*/

#ifndef _PRACTICA3_COMMANDS_H_
#define _PRACTICA3_COMMANDS_H_

/*==================[inclusiones]============================================*/

#include "sapi_datatypes.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/**
* @enum practica3Commands
* @brief Comandos de la practica 3
*/
enum
{
   PRACTICA_3_CMD_ID_TO_MAYUS = 0
,  PRACTICA_3_CMD_ID_TO_MINUS
,  PRACTICA_3_CMD_ID_STACK_MINIMUM
,  PRACTICA_3_CMD_ID_HEAP_AVAILABLE
,  PRACTICA_3_CMD_ID_STATUS_MSG
,  PRACTICA_3_CMD_ID_TO_MEAS_PERFORMANCE
,  PRACTICA_3_CMD_ID_BUTTON_EVENTS
,  PRACTICA_3_CMD_ID_MAX
};

/**
* @def QUEUE_ENTRIES_MAX
* @brief Definicion del tamaño maximo de la cola 
* @note Decimal > 0 
*/
#define QUEUE_ENTRIES_MAX           10                            // Decimal > 0

#include "commands_parser.h"

/**
* @def practica3Frame_t
* @brief Redefinicion de frame_t
*/
typedef frame_t   practica3Frame_t;

/**
* @def practica3CbHandlers_t
* @brief Redefinicion de commands_cbHandlers_t
*/
typedef commands_cbHandlers_t practica3CbHandlers_t;

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/
/**
* @fn bool_t practica3Commands_init(void)
* @brief Inicializacion del parser
* @param Ninguno
* @return bool_t TRUE si la inicializacion fue correcta, FALSE caso contrario.
*/
uint8_t practica3Commands_init(void);

/**
* @def practica3Commands_registerIndHandlers(taskId, ptrTaskCbHandlers)
* @brief Re-definicion del registro de callback handlers del parser
*/
#define practica3Commands_registerIndHandlers(taskId, ptrTaskCbHandlers)  commandsParser_registerIndHandlers(taskId, ptrTaskCbHandlers)
/**
* @def practica3Commands_getMemPoolBlockSize(frame)
* @brief Re-definicion de la obtencion de tamaño del mem pool del parser
*/
#define practica3Commands_getMemPoolBlockSize(frame) commandsParser_getMemPoolBlockSize(frame)
/**
* @def practica3Commands_dataPush(ptrFrame, withMemAlloc)
* @brief Re-definicion del agregado de datos en la cola del parser
*/
#define practica3Commands_dataPush(ptrFrame, withMemAlloc)  commandsParser_dataPush(ptrFrame, withMemAlloc)
/*==================[end of file]============================================*/

#endif /* _PRACTICA3_COMMANDS_H_ */
