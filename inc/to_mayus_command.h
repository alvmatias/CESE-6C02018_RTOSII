/** 
* @file  to_mayus_command.h
* @brief Implementacion del procesador del comando "To Mayus"
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia: 
		Fecha: 17/10/2018
*/

#ifndef _TO_MAYUS_COMMAND_H_
#define _TO_MAYUS_COMMAND_H_

/*==================[inclusiones]============================================*/
#include "sapi_datatypes.h"
#include "practica3_commands.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
* @fn bool_t toMayusCommand_init(void)
* @brief Inicializacion de la tarea del comando y cola de recepcion
* @param Ninguno
* @return bool_t TRUE si la inicializacion fue correcta, FALSE caso contrario.
*/
bool_t toMayusCommand_init(void);


/**
* @fn void toMayusCommand_dataPush(const practica2Frame_t *frame)
* @brief Agregado de un frame a la cola de recepcion del procesador del comando
* @param frame : Frame a agregar en la cola
* @return Nada.
*/
void toMayusCommand_dataPush(const practica3Frame_t *frame);

extern const uint8_t TO_MAYUS_TASK_ID;

/*==================[end of file]============================================*/
#endif /* _TO_MAYUS_COMMAND_H_ */
