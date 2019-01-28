/** 
* @file  to_meas_performance_command.h
* @brief Implementacion del procesador del comando "To Measure Performance"
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
    Licencia: 
    Fecha: 17/10/2018
*/

#ifndef _TO_MEAS_PERFORMANCE_COMMAND_H_
#define _TO_MEAS_PERFORMANCE_COMMAND_H_

/*==================[inclusiones]============================================*/
#include "sapi_datatypes.h"
#include "practica3_commands.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/
/**
* @fn bool_t toMeasPerformanceCommand_init(void)
* @brief Inicializacion de la tarea del comando y cola de recepcion
* @param Ninguno
* @return bool_t TRUE si la inicializacion fue correcta, FALSE caso contrario.
*/
bool_t toMeasPerformanceCommand_init(void);

/**
* @fn void toMeasPerformanceCommand_dataPush(const practica3Frame_t *frame)
* @brief Agregado de un frame a la cola de recepcion del procesador del comando
* @param frame : Frame a agregar en la cola
* @return Nada.
*/
void toMeasPerformanceCommand_dataPush(const practica3Frame_t *frame);

extern const uint8_t TO_MEAS_PERFORMANCE_TASK_ID;

/*==================[end of file]============================================*/
#endif /* _TO_MEAS_PERFORMANCE_COMMAND_H_ */
