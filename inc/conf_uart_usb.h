/**
* @file  conf_uart_usb.h
* @brief Configuracion de la UART.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

#ifndef _CONF_UART_USB_H_
#define _CONF_UART_USB_H_

/*==================[inclusiones]============================================*/

/*==================[macros]=================================================*/

/**
* @def UART_USB_BAUDRATE
* @brief Baud Rate a utilizar por la aplicacion
*/
#define UART_USB_BAUDRATE        115200

/**
* @def UART_USB_RX_BUFFER_SIZE
* @brief Tamaño de buffer de recepcion a utilizar por la aplicacion
*/
#define UART_USB_RX_BUFFER_SIZE  4096

/**
* @def UART_USB_TX_BUFFER_SIZE
* @brief Tamaño de buffer de transmision a utilizar por la aplicacion
*/
#define UART_USB_TX_BUFFER_SIZE  1024

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos internos]========================*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[end of file]============================================*/

#endif /* _CONF_UART_USB_H_ */
