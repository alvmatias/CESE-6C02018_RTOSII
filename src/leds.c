/*============================================================================
 * Autores: Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo 
 * Licencia: 
 * Fecha: 17/10/2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/
#include "leds.h"
#include "conf_leds.h"
#include "fsm.h"
#include "sapi_gpio.h"

/*==================[definiciones y macros]==================================*/
/**
* @struct 
* @brief Informacion de control del led
*/
typedef struct {
   uint32_t id;
} ledControl_t;

/**
* @var static const uint32_t ledsGpioBoardMap[]
* @brief Mapeo de los GPIO's de los leds que contiene el BOARD usado
*/
static const uint32_t ledsGpioBoardMap[] = LEDS_BOARD_GPIO_MAP;

/**
* @def LEDS_MAX
* @brief Cantidad maxima de leds que contiene el BOARD usado
*/
#define LEDS_MAX  sizeof(ledsGpioBoardMap)/sizeof(ledsGpioBoardMap[0])

/**
* @def LED_TURN_OFF
* @brief Led Apagado
*/
#define LED_TURN_OFF   0

/**
* @def LED_TURN_ON
* @brief Led Encendido
*/
#define LED_TURN_ON    !LED_TURN_OFF

/*==================[declaracion de datos internos]=========================*/
/**
* @var static const fsmState_t ledBlinkOnWaitPeriodTimeout[]
* @brief Estado de led a la espera de encenderse
*/
static const fsmState_t ledBlinkOnWaitPeriodTimeout[];

/**
* @var static const fsmState_t ledBlinkOffWaitPeriodTimeout[]
* @brief Estado de led a la espera de apagarse
*/
static const fsmState_t ledBlinkOffWaitPeriodTimeout[];

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de datos internos]=========================*/

/* Variables de control */
/**
* @var static bool_t gInitFlag
* @brief Flag de inicializacion de los leds
*/
static bool_t gInitFlag;

/**
* @var static ledControl_t gLeds[LEDS_MAX]
* @brief Arreglo de leds de la aplicacion
*/
static ledControl_t gLeds[LEDS_MAX];

/*==================[definiciones de datos externos]=========================*/

/*==================[definiciones de funciones internas]=====================*/
/**
 * @fn static void leds_init(void)
 * @brief Inicializacion del módulo de leds
 * @param Ninguno
 * @return Nada
 */
static void leds_init(void) {
   uint8_t i;

   // Inicializar y configurar el arreglo de control de los leds
   for(i = 0;i < LEDS_MAX;i++) {

     // Asignar como id del led el gpio correpondiente
     gLeds[i].id = ledsGpioBoardMap[i];

   }

   gInitFlag = TRUE;
}

/**
* @fn static ledControl_t* getBlinkyLed(const uint32_t ledIndex)
* @brief Obtencion de led
* @param ledIndex : Indice del led a obtener
* @return ledControl_t : Puntero a la estructura del led obtenido
*/
static ledControl_t* getBlinkyLed(const uint32_t ledIndex) {

   if(LEDS_MAX > ledIndex)
      return &gLeds[ledIndex];

   return NULL;
}

/**
* @fn static void ledTurnOn(const uint32_t ledIndex)
* @brief Encendido de led
* @param ledIndex : Indice del led a encender
* @return Nada
*/
static void ledTurnOn(const uint32_t ledIndex) {
   if(gInitFlag) {
      ledControl_t* ledBlinky = getBlinkyLed(ledIndex);

      if(ledBlinky) {

         // Encender el led
         gpioWrite(ledBlinky->id, LED_TURN_ON);
      }
   }
}

/**
* @fn static void ledTurnOff(const uint32_t ledIndex)
* @brief Apagado de led
* @param ledIndex : Indice del led a apagar
* @return Nada
*/
static void ledTurnOff(const uint32_t ledIndex) {
   if(gInitFlag) {
      ledControl_t* ledBlinky = getBlinkyLed(ledIndex);

      if(ledBlinky) {

         // Apagar el led
         gpioWrite(ledBlinky->id, LED_TURN_OFF);
      }
   }
}
/*==================[definiciones de funciones externas]=====================*/

void ledsModuleHandler(event_t * event){

	switch(event->signal){
		case SIGNAL_INIT:
			leds_init();
			break;

		case SIGNAL_BUTTON_PRESSED:
			ledTurnOn(event->value);
			break;

		case SIGNAL_BUTTON_RELEASED:
			ledTurnOff(event->value);
			break;

		default:  // Ignoro todas las otras señales
			break;
	}
}


/*==================[fin del archivo]========================================*/

