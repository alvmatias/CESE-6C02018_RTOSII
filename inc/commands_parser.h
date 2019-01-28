/**
* @file  commands_parser.h
* @brief Implementacion del parser de los paquetes.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/*==================[inclusiones]============================================*/
#include "sapi_datatypes.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/**
* @struct frame_t
* @brief Estructura de datos para comunicar paquetes entre el parser y los procesadores de los comandos
* @note Tipo de I/O Vectorizado
*/
typedef struct {
   uint8_t cmdId;
   uint32_t minimumStackSize;
   uint8_t frameSize;
   uint8_t *frameData;
} frame_t;

/**
* @def void (*cmdDataPush_t)(const frame_t *frame)
* @brief Definicion del callback de agregado de datos en la cola de los procesadores de los comandos
*/
typedef void (*cmdDataPush_t)(const frame_t *frame);

/**
* @def void (*cmdDataPop_t)(frame_t *const frame)
* @brief Definicion del callback de obtencion de datos de la cola de los procesadores de los comandos
*/
typedef void (*cmdDataPop_t)(frame_t *const frame);

/**
* @struct commands_t
* @brief Estructura de datos que representa un comando
*/
typedef struct{
	uint8_t       cmdId;
	cmdDataPush_t pfnDataPush;
	cmdDataPop_t  pfnDataPop;
} commands_t;

/**
* @def void (*asyncOp_cbHandler_t)(uint8_t *frame)
* @brief Definicion de callback handler para medicion de performance de paquete
*/
typedef void (*asyncOp_cbHandler_t)(uint8_t *frame);

/**
* @struct commands_cbHandlers_t
* @brief Callback Handlers para medicion de performace de paquete
*/
typedef struct {
   asyncOp_cbHandler_t rcvSTX_cbHandler;
   asyncOp_cbHandler_t rcvETX_cbHandler;
   asyncOp_cbHandler_t sndSTX_cbHandler;
   asyncOp_cbHandler_t sndETX_cbHandler;
} commands_cbHandlers_t;

/*==================[declaraciones de datos internos]========================*/

/*==================[declaraciones de datos externos]========================*/
extern const commands_t g_commandsArr[];
extern const uint8_t COMMANDS_PARSER_TX_QUEUE_ENTRIES_MAX;
extern const uint8_t COMMANDS_PARSER_INVALID_CMD_ID;
extern const uint8_t COMMANDS_PARSER_MINIMUM_STACK_CMD_ID;
extern const uint8_t COMMANDS_PARSER_AVAILABLE_HEAP_CMD_ID;

/*==================[declaraciones de funciones internas]====================*/
/**
* @fn bool_t commandsParser_init(void)
* @brief Inicializacion del parser, mempool y driver de la UART
* @param Ninguno
* @return bool_t TRUE si la inicializacion fue correcta, FALSE caso contrario.
*/
bool_t commandsParser_init(void);

/**
* @fn bool_t commandsParser_registerIndHandlers(const uint8_t taskId, const commands_cbHandlers_t * observerCbHandlers)
* @brief Registro de callback handlers para medicion de performance de paquetes
* @param taskId               : Id de la tarea que registra los callbacks
* @param observerCbHandlers   : Puntero a la estructura que contiene los distintos callbacks
* @return bool_t TRUE si fue posible registrarlos, FALSE caso contrario.
*/
bool_t commandsParser_registerIndHandlers(const uint8_t taskId, const commands_cbHandlers_t * observerCbHandlers);

/**
* @fn uint32_t commandsParser_getMemPoolBlockSize(const frame_t *frame);
* @brief Obtencion del tamaño de bloque del mem pool del parser de acuerdo a la direccion de los datos del frame pasado
* @param frame : Frame conteniendo los datos alocados en el mem pool
* @return uint32_t Tamaño de bloque del mem pool donde estan alojados los datos del frame pasado como parametros
*/
uint32_t commandsParser_getMemPoolBlockSize(const frame_t *frame);

/**
* @fn bool_t commandsParser_dataPush(frame_t * frame, bool_t withMemAlloc)
* @brief Agregado de dato en la cola del parser
* @param frame : Frame a ser agregado
* @param withMemAlloc : Booleano que indica si los datos deben ser previamente alocados antes de ser agregados en la cola o no
* @return bool_t TRUE si pudo agregarse y/o alocarse el frame, FALSE caso contrario
*/
bool_t commandsParser_dataPush(frame_t * frame, bool_t withMemAlloc);

/*==================[declaraciones de funciones externas]====================*/

/*==================[end of file]============================================*/

#endif /* _COMMANDS_H_ */
