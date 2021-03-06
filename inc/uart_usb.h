/** 
* @file  uart_usb.h
* @brief Implementacion del driver de la UART 2 (UART USB) de la EDU-CIAA
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
    Licencia: 
    Fecha: 17/10/2018
*/

#ifndef _UART_USB_H_
#define _UART_USB_H_

/*==================[inclusiones]============================================*/

#include "sapi_datatypes.h"

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
 * @fn void uartUsbInit(void)
 * @brief Inicializa el periferico de la uart que esta mapeado al conector USB de la board EDU-CIAA-NXP.
 * @param Ninguno
 * @return Nada
 */
void uartUsbInit(void);

/**
 * @fn bool_t uartUsbReadByte(uint8_t* byte)
 * @brief Obtiene un byte del buffer circular de recepcion de la UART.
 * @param byte : Puntero donde almacenar el byte recibido
 * @return bool_t TRUE si se recibio un byte, FALSE caso contrario.
 */
bool_t uartUsbReadByte(uint8_t* byte);

/**
 * @fn uint8_t uartUsbReadData(uint8_t* buff, const uint8_t expBytesLen, const uint32_t timeout)
 * @brief Obtiene un conjunto de bytes del buffer circular de recepcion de la UART.
 * @param buff 			: Puntero al buffer de la aplicacion donde almacenar los bytes leidos
 * @param expBytesLen 	: Cantidad maxima de bytes que se deberan almacenar en el buffer de la aplicacion
 * @param timeout 		: Tiempo maximo de espera en ms
 * @return uint8_t Numero de bytes leidos
 */
uint8_t uartUsbReadData(uint8_t* buff, const uint8_t expBytesLen, const uint32_t timeout);

/**
 * @fn bool_t uartUsbWriteByte(const uint8_t byte)
 * @brief Almacena un byte en el buffer circular de transmision de la UART.
 * @param byte 	: Dato a almacenar en el buffer circular de transmision de la UART
 * @return bool_t TRUE si el byte pudo ser almacenado, FALSE caso contario.
*/
bool_t uartUsbWriteByte(const uint8_t byte);

/**
 * @fn bool_t uint8_t uartUsbWriteData(const uint8_t* buff, const uint32_t len)
 * @brief Almacena un conjunto de bytes en el buffer circular de transmision de la UART.
 * @param buff  : Puntero al buffer de la aplicacion con bytes a escribir
 * @param len 	: Cantidad de bytes a escribir
 * @return uint8_t Numero de bytes escritos en el buffer circular de transmision de la UART
*/
uint8_t uartUsbWriteData(const uint8_t* buff, const uint32_t len);

/**
 * @fn uint8_t uartUsbWriteString(const uint8_t* string)
 * @brief Almacena un string en el buffer circular de transmision de la UART.
 * @param buff 	: Puntero al string de la aplicacion escribir
 * @return uint8t_t Numero de bytes escritos en el buffer circular de transmision de la UART
*/
uint8_t uartUsbWriteString(const uint8_t* string);

/**
 * @fn uint8_t uartUsbWriteVariadicString(const uint8_t* format, ...)
 * @brief Escribe un string de largo variable en el buffer circular de transmision de la UART.
 * @param buff 	: Puntero al string de la aplicacion escribir
 * @return uint8t_t Numero de bytes escritos en el buffer circular de transmision de la UART
*/
uint8_t uartUsbWriteVariadicString(const uint8_t* format, ...);

/*==================[end of file]============================================*/
#endif /* _uart_usb_H_ */

