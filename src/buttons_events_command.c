/*============================================================================
 * Autores: Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo 
 * Licencia: 
<<<<<<< HEAD
 * Fecha: 17/10/2018
=======
 * Fecha: 22/09/2018
>>>>>>> 3e5ef68164c6a65e7166dd6acb26ec090d5588e3
 *===========================================================================*/

/*==================[inlcusiones]============================================*/
#include <string.h>
#include "conf_buttons.h"
#include "button_events_command.h"
#include "practica3_commands.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/*==================[definiciones y macros]==================================*/


/**
* @var static const uint32_t buttonsGpioBoardMap[]
* @brief Mapeo de los GPIO's de los botones que contiene el BOARD usado
*/
static const uint32_t buttonsGpioBoardMap[] = BUTTONS_BOARD_GPIO_MAP;

/**
* @def BUTTONS_MAX
* @brief Cantidad maxima de botones que contiene el BOARD usado
*/
#define BUTTONS_MAX sizeof(buttonsGpioBoardMap)/sizeof(buttonsGpioBoardMap[0])


/**
* @def BUTTON_EVENTS_MSG_SIZE_MAX
* @brief Definicion del tamano maximo del buffer del mensaje
*/
#define BUTTON_EVENTS_MSG_SIZE_MAX 256

/**
* @struct timeSampling_t
* @def Estructura de tiempos de presion y liberacion de un boton
*/
typedef struct {
    TickType_t pressTimestamp;
    TickType_t releaseTimestamp;
} timeSampling_t;

/*==================[definiciones de datos internos]=========================*/

/**
* @var timeSampling_t g_buttonsTimestampsArr[BUTTONS_MAX]
* @brief Datos de los tiempos de cada boton de la aplicacion
*/
timeSampling_t g_buttonsTimestampsArr[BUTTONS_MAX];

/**
* @var static uint8_t g_buttonEventsMsgBuf[BUTTON_EVENTS_MSG_SIZE_MAX]
* @brief Buffer para alocar temporariamente el mensaje de reporte generado
*/
static uint8_t g_buttonEventsMsgBuf[BUTTON_EVENTS_MSG_SIZE_MAX];

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/
void reportModuleHandler(event_t *event) {

    switch(event->signal)  {

       case SIGNAL_INIT:
       {
          // Nothing to do here..
       } break;

        case SIGNAL_BUTTON_PRESSED:
        {
           g_buttonsTimestampsArr[event->value].pressTimestamp = xTaskGetTickCount();
        } break;

        case SIGNAL_BUTTON_RELEASED:
        {
           g_buttonsTimestampsArr[event->value].releaseTimestamp = xTaskGetTickCount();

           /* Generate token information frame and insert it into transmission queue with memory allocation */
           uint8_t strSize = (uint8_t) snprintf((char *) g_buttonEventsMsgBuf,
                                      sizeof(g_buttonEventsMsgBuf),
                                      "Boton pulsado: %u, Tiempo presionado: %u ms",
                                      (unsigned int) (event->value + 1),
                                      (unsigned int) (g_buttonsTimestampsArr[event->value].pressTimestamp > g_buttonsTimestampsArr[event->value].releaseTimestamp ?
                                                     g_buttonsTimestampsArr[event->value].pressTimestamp - g_buttonsTimestampsArr[event->value].releaseTimestamp :
                                                     g_buttonsTimestampsArr[event->value].releaseTimestamp - g_buttonsTimestampsArr[event->value].pressTimestamp));

           practica3Frame_t frame = {
                                       .cmdId              = PRACTICA_3_CMD_ID_BUTTON_EVENTS
                                   ,   .minimumStackSize   = uxTaskGetStackHighWaterMark(NULL)   /* NULL to query its own stack */
                                   ,   .frameSize          = strSize
                                   ,   .frameData          = g_buttonEventsMsgBuf
                                    };

           practica3Commands_dataPush(&frame, TRUE);

        } break;

        default:  // Ignoro todas las otras señales
            break;
    }
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
