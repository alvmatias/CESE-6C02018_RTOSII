/**
* @file  seniales.h
<<<<<<< HEAD
* @brief Definicion de las señales de la practica 3
=======
* @brief Definicion de los comandos de la practica 3
>>>>>>> 3e5ef68164c6a65e7166dd6acb26ec090d5588e3
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
      Licencia:
      Fecha: 17/10/2018
*/

#ifndef _SENIALES_H_
#define _SENIALES_H_

/*==================[inclusiones]============================================*/

#include "sapi_datatypes.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/**
* @enum
* @brief Definicion de las posibles señales de la aplicacion
* @note Definir debajo de SIGNAL_INIT todas las señales necesarias
*/
enum {
    SIGNAL_INIT      = 0x00   /**< Señal de inicializacion de modulo */
/* Agregar debajo las seniales de la aplicacion */
,   SIGNAL_TIMEOUT				/**< Señal de timeout */
,   SIGNAL_BUTTON_PRESSED		/**< Señal de boton presionado */
,   SIGNAL_BUTTON_RELEASED		/**< Señal de boton liberado */

} seniales_t;

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

#endif /* _SENIALES_H_ */
