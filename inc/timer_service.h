/** 
* @file  timer_service.h
* @brief Manejo de timer de hardware del lpc4337 y timers de software.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia: 
		Fecha: 17/10/2018
*/

#ifndef _TIMER_SERVICE_H_
#define _TIMER_SERVICE_H_

/*==================[inclusions]=============================================*/
#include "sapi_peripheral_map.h"

/*==================[macros and definitions]=================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
* @fn bool_t timerService_initHw()
* @brief Inicializacion del timer de hardware y su estructura de datos interna
* @param Ninguno
* @return bool_t TRUE si la inicializacion fue correcta, FALSE caso contrario
*/
bool_t timerService_initHw(void);

/**
* @fn bool_t timerService_readHwCounterValue(uint32_t * timerCount, uint32_t * overflowCount)
* @brief Lectura del valor actual del contador del timer de hardware
* @param timerUs    : Tiempo en microsegundos del valor del contador del timer de hardware
* @return bool_t TRUE si la lectura fue correcta, FALSE caso contrario
* @note NO bloqueante
*/
bool_t timerService_readHwCounterValue(uint64_t *timerUs);

/**
* @fn bool_t timerService_createSwTimerOneSHot(void *receptor, uint32_t timeoutMs)
* @brief Creacion de timer de software
* @param receptor  : Modulo que recibe los eventos de timeout del timer de software
* @param timeoutMs : Tiempo que debe transcurrir hasta el timeout
* @return bool_t TRUE si la creacion fue correcta, FALSE caso contrario
* @note El timeout se dispara una unica vez
*/
bool_t timerService_createSwTimerOneSHot(void *receptor, uint32_t timeoutMs);

/**
* @fn bool_t timerService_createSwTimerPeriodic(void *receptor, uint32_t timeoutMs)
* @brief Creacion de timer de software
* @param receptor  : Modulo que recibe los eventos de timeout del timer de software
* @param timeoutMs : Tiempo que debe transcurrir hasta el timeout
* @return bool_t TRUE si la creacion fue correcta, FALSE caso contrario
* @note El timeout se periodicamente cada timeoutMs
*/
bool_t timerService_createSwTimerPeriodic(void *receptor, uint32_t timeoutMs);

/**
* @fn bool_t timerService_createSwTimerOneSHot(void *receptor, uint32_t timeoutMs)
* @brief Destruccion de timer de software
* @param receptor  : Modulo que recibe los eventos de timeout del timer de software
* @return Nada
*/

/*==================[end of file]============================================*/
#endif /* _TIMER_SERVICE_H_ */
