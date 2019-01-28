/**
* @file  conf_leds.h
* @brief Configuraciones para el manejo de los leds
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/
#ifndef _CONF_LEDS_H_
#define _CONF_LEDS_H_

/*==================[inclusiones]============================================*/
#include "sapi_peripheral_map.h"
/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/**
* @def BUTTONS_BOARD_GPIO_MAP
* @brief Definicion del listado de los leds existentes en el BOARD utilizado
* @note Agregar los necesarios debajo
*/
#define LEDS_BOARD_GPIO_MAP	{			   \
                                   LEDB	\
                                ,  LED1  	\
                                ,  LED2  	\
                                ,  LED3  	\
								    	};

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/


/*==================[end of file]============================================*/
#endif /* _CONF_LEDS_H_ */
