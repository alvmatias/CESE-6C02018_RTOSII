/** 
* @file  app.c
* @brief Manejo de la aplicacion.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
      Licencia: 
      Fecha: 17/10/2018
*/

/*==================[inlcusiones]============================================*/
#include "sapi_board.h"

#include "app.h"
#include "practica3_commands.h"
#include "to_minus_command.h"
#include "to_mayus_command.h"
#include "to_meas_performance_command.h"
#include "events_framework.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/* Match application modules */
/**
* @var const void *LED_MODULE_RECEPTOR
* @brief Modulo receptor de los eventos de los leds
*/
const void *LED_MODULE_RECEPTOR     = &g_efModulesArr[0];

/**
* @var const void *BUTTON_MODULE_RECEPTOR
* @brief Modulo receptor de los eventos de los botones
*/
const void *BUTTON_MODULE_RECEPTOR  = &g_efModulesArr[1];

/**
* @var const void *REPORT_MODULE_RECEPTOR
* @brief Modulo generador de reportes de eventos de botones
*/
const void *REPORT_MODULE_RECEPTOR  = &g_efModulesArr[2];


/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/
bool_t appInit() {

   /* Configure board */
   boardConfig();

   /* Initialize practica3_commands */
   if(!practica3Commands_init())
      return FALSE;

   /* Initialize "to_minus" command processor */
   if(!toMinusCommand_init())
      return FALSE;

   /* Initialize "to_mayus" command processor */
   if(!toMayusCommand_init())
      return FALSE;

   /* Initialize "to_meas_performance" command processor */
   if(!toMeasPerformanceCommand_init())
      return FALSE;

   /* Initialize events framework */
   if(!eventsFramework_init())
      return FALSE;

   return TRUE;
}

void appRun() {

   /* Initialize scheduler */
   vTaskStartScheduler();
}

void appAssert() {

   /* Should never get here! */
   while(TRUE)
         ;
}


/*==================[fin del archivo]========================================*/
