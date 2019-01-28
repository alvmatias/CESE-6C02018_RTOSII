/**
* @file  conf_buttons.h
* @brief Configuraciones para el manejo de los botones
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/
#ifndef _CONF_BUTTONS_H_
#define _CONF_BUTTONS_H_

/*==================[inclusiones]============================================*/

#include "sapi_peripheral_map.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/**
* @def BUTTONS_BOARD_GPIO_MAP
* @brief Definicion del listado de los botones existentes en el BOARD utilizado
* @note Agregar los necesarios debajo
*/
#define BUTTONS_BOARD_GPIO_MAP	{			\
									         TEC1	\
									       , TEC2  \
									       ,	TEC3  \
									       ,	TEC4	\
						               };


/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[end of file]============================================*/
#endif /* _CONF_BUTTONS_H_ */
