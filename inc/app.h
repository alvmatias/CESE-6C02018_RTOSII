/**
* @file  app.h
* @brief Manejo de la aplicacion.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/
#ifndef _APP_H_
#define _APP_H_

/*==================[inclusiones]============================================*/

#include "sapi_datatypes.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
 * @fn bool_t appInit(void)
 * @brief Inicializacion de cada modulo de la aplicacion
 * @param Ninguno
 * @return bool_t TRUE si la inicialziacion fue correcta, FALSE caso contrario.
 */
bool_t appInit(void);

/**
 * @fn void appRun(void)
 * @brief Ejecucion del Scheduler del Sistema Operativo.
 * @param Ninungo
 * @return void Nada.
 */
void appRun(void);

/**
 * @fn void appAssert(void)
 * @brief Asegura la correcta inicializacion de la aplicacion
 * @param Ninguno
 * @return void Nada.
 */
void appAssert(void);

/*==================[c++]====================================================*/

/*==================[end of file]============================================*/
#endif /* _APP_H_ */
