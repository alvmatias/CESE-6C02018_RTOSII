/*============================================================================
 * Autores: Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo 
 * Licencia: 
 * Fecha: 17/10/2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "conf_buttons.h"
#include "buttons.h"
#include "events_framework.h"
#include "fsm.h"
#include "timer_service.h"
#include "sapi_gpio.h"

/*==================[definiciones y macros]==================================*/

/**
* @struct button_t
* @brief Informacion de control de un boton
*/
typedef struct {
	uint32_t id;         /**< Id del boton */
	fsmState_t* state;   /**< Estado actual dentro de la fms */
} button_t;

/**
* @var static const uint32_t buttonsGpioBoardMap[]
* @brief Mapeo de los GPIO's de los botones que contiene el BOARD usado
*/
static const uint32_t buttonsGpioBoardMap[] = BUTTONS_BOARD_GPIO_MAP;

/**
* @def BUTTONS_MAX
* @brief Cantidad maxima de botones que contiene el BOARD usado
*/
#define BUTTONS_MAX	sizeof(buttonsGpioBoardMap)/sizeof(buttonsGpioBoardMap[0])

/**
* @def TRANSITION_WAIT_INTERVAL_MS
* @brief Tiempo de transicion entre estados de la fms
* @note En milisegundos
*/
#define TRANSITION_WAIT_INTERVAL_MS  50ul  

/*==================[declaracion de datos internos]=========================*/
/**
* @var static const fsmState_t buttonReleased[]
* @brief Estado de boton liberado
*/
static const fsmState_t buttonReleased[];

/**
* @var static const fsmState_t buttonTransitionPressing[]
* @brief Estado de boton en flanco descendente
*/
static const fsmState_t buttonTransitionPressing[];

/**
* @var static const fsmState_t buttonPressed[]
* @brief Estado de boton presionado
*/
static const fsmState_t buttonPressed[];

/**
* @var static const fsmState_t buttonTransitionReleasing[]
* @brief Estado de boton en flanco ascendente
*/
static const fsmState_t buttonTransitionReleasing[];

/*==================[declaraciones de funciones internas]====================*/
static void doNothing(void *arg);
static void sendButtonStatusAsPressed(void *arg);
static void sendButtonStatusAsReleased(void *arg);

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de datos internos]=========================*/

/* Tabla de la maquina de estados finita */

DEFINE_FSM_STATE(buttonReleased,
                 {BUTTON_DOWN, buttonTransitionPressing, doNothing})

DEFINE_FSM_STATE(buttonTransitionPressing,
                 {BUTTON_UP, buttonReleased, doNothing}
                 ,{BUTTON_DOWN, buttonPressed, sendButtonStatusAsPressed})

DEFINE_FSM_STATE(buttonPressed,
                 {BUTTON_UP, buttonTransitionReleasing, doNothing})

DEFINE_FSM_STATE(buttonTransitionReleasing,
                 {BUTTON_UP, buttonReleased, sendButtonStatusAsReleased}
                 ,{BUTTON_DOWN, buttonPressed, doNothing})

/* Variables de control */
/**
* @var static bool_t g_initFlag
* @brief Flag de inicializacion de los botones
*/
static bool_t g_initFlag;

/**
* @var static button_t g_buttonsArr[BUTTONS_MAX]
* @brief Arreglo de botones de la aplicacion
*/
static button_t g_buttonsArr[BUTTONS_MAX];

/*==================[definiciones de datos externos]=========================*/

/*==================[definiciones de funciones internas]=====================*/
/**
* @fn static bool_t eventGenerator(button_t* button, uint8_t* event)
* @brief Generador de evento
* @param button : Boton que genera el evento
* @param event  : Evento generado
* @return bool_t TRUE si fue posible leer el estado del boton, FALSE caso contrario
*/
static bool_t eventGenerator(button_t* button, uint8_t* event) {

   if(button && event) {
		// Leer el nivel del boton y generar el evento correspondiente
    *event = (0 == gpioRead(button->id) ? BUTTON_DOWN : BUTTON_UP);
    return TRUE;
	}

	return FALSE;
}


/* Funciones de accion de la maquina de estados finita */
/**
* @fn static void doNothing(void *arg)
* @brief Funcion que no realiza nada
* @return Nada
*/
static void doNothing(void *arg) {
	(void) arg;
}

/**
* @fn static uint8_t getButtonIndex(const uint32_t buttonId)
* @brief Obtencion de indice de boton dentro del arreglo de botones
* @param buttonId : Id de Boton sobre el cual buscar el indice
* @return El indice leido si el boton existe dentro del arreglo de botones, 0 caso contrario
*/

static uint8_t getButtonIndex(const uint32_t buttonId) {

  uint8_t i;

  for(i = 0;i < BUTTONS_MAX;i++) {
     if(buttonId == g_buttonsArr[i].id)
       return i;
  }

  return 0;
}

/**
* @fn static void sendButtonStatusAsPressed(void *arg)
* @brief Envio del estado presionado de boton a los modulos de interes
* @param arg : Id del boton
* @return Nada
*/
static void sendButtonStatusAsPressed(void *arg) {

   uint32_t buttonId = *((uint32_t *) arg);
   uint32_t buttonIndex = getButtonIndex(buttonId);

   /* Push event to led module */
   events_pushEvent(LED_MODULE_RECEPTOR, SIGNAL_BUTTON_PRESSED, buttonIndex);

   /* Push event to report module */
   events_pushEvent(REPORT_MODULE_RECEPTOR, SIGNAL_BUTTON_PRESSED, buttonIndex);
}

/**
* @fn static void sendButtonStatusAsReleased(void *arg)
* @brief Envio del estado liberado de boton a los modulos de interes
* @param arg : Id del boton
* @return Nada
*/
static void sendButtonStatusAsReleased(void *arg) {

  uint32_t buttonId = *((uint32_t *) arg);
  uint32_t buttonIndex = getButtonIndex(buttonId);

  /* Push event to led module */
  events_pushEvent(LED_MODULE_RECEPTOR, SIGNAL_BUTTON_RELEASED, buttonIndex);

  /* Push event to report module */
  events_pushEvent(REPORT_MODULE_RECEPTOR, SIGNAL_BUTTON_RELEASED, buttonIndex);
}

/**
 * @fn static void buttons_init(void)
 * @brief Inicializacion del módulo de botones que se encarga de implementar por software el antirebote al ser pulsado
 * @param Ninguno
 * @return Nada
 */
static void buttons_init(void) {

  /* Initialize and configure buttons array */
  for(uint8_t i = 0;i < BUTTONS_MAX;i++) {

    /* Asign button id to gpio */
    g_buttonsArr[i].id = buttonsGpioBoardMap[i];

    g_buttonsArr[i].state = (fsmState_t *) buttonReleased;
  }

  /* Create software timer */
  if(timerService_createSwTimerPeriodic(BUTTON_MODULE_RECEPTOR, TRANSITION_WAIT_INTERVAL_MS))
    g_initFlag = TRUE;
}
/*==================[definiciones de funciones externas]=====================*/
void buttonsModuleHandler(event_t * event){

  switch(event->signal){

    case SIGNAL_INIT:
    {
       buttons_init();
    } break;

    case SIGNAL_TIMEOUT:
    {
       if(g_initFlag) {
         uint8_t event;

         // Actualizar el estado de la maquina de estados finita de cada boton
         for(uint8_t i = 0;i < BUTTONS_MAX;i++) {

            // Obtener el evento actual
           if(eventGenerator(&g_buttonsArr[i], &event))
             fsm(&g_buttonsArr[i].state, event, &g_buttonsArr[i].id);
         }
       }
    } break;

    default:  // Ignoro todas las otras señales
      break;
  }
}

/*==================[fin del archivo]========================================*/

