/** 
* @file  practica3_commands.c
* @brief Definicion de los comandos de la practica 3
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia: 
		Fecha: 17/10/2018
*/

/*==================[inlcusiones]============================================*/
#include "practica3_commands.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/**
* @var const uint8_t COMMANDS_PARSER_TX_QUEUE_ENTRIES_MAX
* @brief Definicion de la cantidad maxima de entradas para la cola de transmision del parser
*/
const uint8_t COMMANDS_PARSER_TX_QUEUE_ENTRIES_MAX = QUEUE_ENTRIES_MAX;

/**
* @var const uint8_t COMMANDS_PARSER_INVALID_CMD_ID
* @brief Definicion de comando invalido para el parser
*/
const uint8_t COMMANDS_PARSER_INVALID_CMD_ID = PRACTICA_3_CMD_ID_MAX;

/**
* @var const uint8_t COMMANDS_PARSER_MINIMUM_STACK_CMD_ID
* @brief Definicion de comando de reporte de minimo stack disponible para el parser
*/
const uint8_t COMMANDS_PARSER_MINIMUM_STACK_CMD_ID = PRACTICA_3_CMD_ID_STACK_MINIMUM;

/**
* @var const uint8_t COMMANDS_PARSER_AVAILABLE_HEAP_CMD_ID
* @brief Definicion de comando heap disponible para el parser
*/
const uint8_t COMMANDS_PARSER_AVAILABLE_HEAP_CMD_ID = PRACTICA_3_CMD_ID_HEAP_AVAILABLE;

/* Include below file functions handlers */
#include "to_mayus_command.h"
#include "to_minus_command.h"
#include "to_meas_performance_command.h"

/**
* @var const commands_t g_commandsArr[]
* @brief Comandos de la practica 3
*/
const commands_t g_commandsArr[] = {

           /* CMD_ID */                         /* PFN_DATA_PUSH*/
   {PRACTICA_3_CMD_ID_TO_MAYUS,              toMayusCommand_dataPush,            }
,  {PRACTICA_3_CMD_ID_TO_MINUS,              toMinusCommand_dataPush,            }
,  {PRACTICA_3_CMD_ID_TO_MEAS_PERFORMANCE,   toMeasPerformanceCommand_dataPush   }
,  {PRACTICA_3_CMD_ID_MAX,                   0                                   }
};

/* Asignar ID's a cada tarea dentro de la aplicacion inmediatamente aqui debajo */
/**
* @var uint8_t TO_MAYUS_TASK_ID
* @brief Id correspondiente a la tarea del procesador de to_mayus_command
*/
const uint8_t TO_MAYUS_TASK_ID = 0;

/**
* @var uint8_t TO_MINUS_TASK_ID
* @brief Id correspondiente a la tarea del procesador de to_minus_command
*/
const uint8_t TO_MINUS_TASK_ID = 1;

/**
* @var uint8_t TO_MEAS_PERFORMANCE_TASK_ID
* @brief Id correspondiente a la tarea del procesador de to_meas_performance_command
*/
const uint8_t TO_MEAS_PERFORMANCE_TASK_ID = 2;

//const uint8_t BUTTON_EVENTS_TASK_ID = 3;
/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/
uint8_t practica3Commands_init() {

   if(!commandsParser_init())
      return FALSE;

   return TRUE;
}

/*==================[fin del archivo]========================================*/
