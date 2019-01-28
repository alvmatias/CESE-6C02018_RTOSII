/** 
* @file  to_meas_performance_command.c
* @brief Implementacion del procesador del comando "To Measure Performance"
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
    Licencia: 
    Fecha: 17/10/2018
*/

/*==================[inlcusiones]============================================*/
#include <string.h>
#include "to_meas_performance_command.h"
#include "qmpool.h"
#include "timer_service.h"
#include "fsm.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/*==================[declaraciones de funciones internas]====================*/


/**
* @var static const fsmState_t waitingFirstByteArrival[]
* @brief Estado de espera de llegada del primer byte
*/
static const fsmState_t waitingFirstByteArrival[];

/**
* @var static const fsmState_t waitingLastByteArrival[]
* @brief Estado de espera de llegada del ultimo byte
*/
static const fsmState_t waitingLastByteArrival[];

/**
* @var static const fsmState_t waitingFramePop[]
* @brief Estado de espera de obtencion del frame de la cola de recepcion
*/
static const fsmState_t waitingFramePop[];

/**
* @var static const fsmState_t waitingFramePush[]
* @brief Estado de espera de agregado del frame en la cola de transmision
*/
static const fsmState_t waitingFramePush[];

/**
* @var static const fsmState_t waitingFirstByteDeparture[]
* @brief Estado de espera de envio del primer byte
*/
static const fsmState_t waitingFirstByteDeparture[];

/**
* @var static const fsmState_t waitingLastByteDeparture[]
* @brief Estado de espera de envio del ultimo byte
*/
static const fsmState_t waitingLastByteDeparture[];

static void registerTimeAction(void *arg);

/* Observer callback functions */
static void arrivalTime_Cb(uint8_t *payload);
static void receptionTime_Cb(uint8_t *payload);
static void exitTime_Cb(uint8_t *payload);
static void transmissionTime_Cb(uint8_t *payload);

/*==================[definiciones y macros]==================================*/
/**
* @struct token_t
* @brief Estructura de datos del token de medicion de performance
*/
typedef struct token {
   /* Internal data */
   struct token *next;
   fsmState_t *state;

   /* Token parameters */
   uint32_t packetId;
   uint8_t *payload;
   uint32_t arrivalTime;
   uint32_t receptionTime;
   uint32_t initialTime;
   uint32_t endTime;
   uint32_t exitTime;
   uint32_t transmissionTime;
   uint16_t frameSize;
   uint16_t usedMemorySize;
} token_t;

/**
* @enum smEvent_t
* @brief Eventos de la maquina de estados finita
*/
typedef enum {
  FIRST_BYTE_ARRIVAL_EVENT = 0x01
, LAST_BYTE_ARRIVAL_EVENT
, FRAME_POP_EVENT
, FRAME_PUSH_EVENT
, FIRST_BYTE_SENT_EVENT
, LAST_BYTE_SENT_EVENT
} smEvent_t;

/***********************
 * State machine TABLE *
 ***********************/

DEFINE_FSM_STATE(waitingFirstByteArrival,
                 {FIRST_BYTE_ARRIVAL_EVENT, waitingLastByteArrival, registerTimeAction});

DEFINE_FSM_STATE(waitingLastByteArrival,
                 {LAST_BYTE_ARRIVAL_EVENT, waitingFramePop, registerTimeAction});

DEFINE_FSM_STATE(waitingFramePop,
                 {FRAME_POP_EVENT, waitingFramePush, registerTimeAction});

DEFINE_FSM_STATE(waitingFramePush,
                 {FRAME_PUSH_EVENT, waitingFirstByteDeparture, registerTimeAction});

DEFINE_FSM_STATE(waitingFirstByteDeparture,
                 {FIRST_BYTE_SENT_EVENT, waitingLastByteDeparture, registerTimeAction});

DEFINE_FSM_STATE(waitingLastByteDeparture,
                 {LAST_BYTE_SENT_EVENT, waitingFirstByteArrival, registerTimeAction});
/**
* @def PERFORMANCE_MSG_SIZE_MAX
* @brief Tamaño maximo del mensaje de reporte de medicion de performance
*/
#define PERFORMANCE_MSG_SIZE_MAX 255

/**
* @var static const practica2CbHandlers_t g_observerCbs
* @brief Estructura que contiene los callbacks de medicion de performance
*/
static const practica3CbHandlers_t g_observerCbs = {
   .rcvSTX_cbHandler = arrivalTime_Cb
,  .rcvETX_cbHandler = receptionTime_Cb
,  .sndSTX_cbHandler = exitTime_Cb
,  .sndETX_cbHandler = transmissionTime_Cb
};

/*==================[definiciones de datos internos]=========================*/

/**
* @var static QueueHandle_t g_rxQueue_Handler 
* @brief Handler de la cola de recepcion del comando
*/
static QueueHandle_t g_rxQueue_Handler = NULL;

/* Memory pool size definitions */
/**
* @def MEM_POOL_COUNT
* @brief Cantidad de mem pools
* @note Decimal > 0
*/
#define MEM_POOL_COUNT     1                             

/**
* @def MEM_POOL_SIZE
* @brief Tamaño de cada mem pool
* @note En bytes
*/  
#define MEM_POOL_SIZE        256

/* Memory pool space */
/**
* @var g_memPool[MEM_POOL_COUNT * MEM_POOL_SIZE]
* @brief Espacio de memoria reservado para los memory pools 
*/
static uint8_t g_memPool[MEM_POOL_COUNT * MEM_POOL_SIZE];

/**
* @var static QMPool g_memPoolHanlder
* @brief Handler del mem pool
*/
static QMPool g_memPoolHanlder;

/**
* @var static token_t *g_activeTokenList
* @brief Lista de los tokens creados esperando a ser procesados
*/
static token_t *g_activeTokenList = NULL;

/**
* @var static token_t *g_currentPassiveToken
* @brief Lista de los tokens a ser agregados en activeTokenList
*/
static token_t *g_currentPassiveToken = NULL;

/**
* @var static uint8_t g_performanceMsgBuf[PERFORMANCE_MSG_SIZE_MAX]
* @brief Buffer de alocacion temporaria de los mensajes de medicion de performance
*/
static uint8_t g_performanceMsgBuf[PERFORMANCE_MSG_SIZE_MAX];

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/
/**
* @fn static uint32_t getPacketId(void)
* @brief Obtencion e incremento del numero de paquete actual
* @param Ninguno
* @return uint32_t Numero de Paquete
*/
static uint32_t getPacketId(void) {
   static uint32_t packetId = 0;

   return packetId++;
}

/**
* @fn static void placeTokenIntoActiveTokenList(token_t *token)
* @brief Agregado de token en la lista de tokens a la espera de ser procesados
* @param token : Token a ser agregado 
* @return Nada
*/
static void placeTokenIntoActiveTokenList(token_t *token)
{
   if(NULL != token) {

      /* Verify it doesn't exists into list */
      for(token_t *t = g_activeTokenList;NULL != t;t = t->next) {
         if(t == token)
            return;
      }

      token->next = g_activeTokenList;
      g_activeTokenList = token;
   }
}

/**
* @fn static void deleteTokenFromActiveTokenList(token_t *token)
* @brief Eliminacion de token en la lista de tokens a la espera de ser procesados
* @param token : Token a ser eliminado 
* @return Nada
*/
static void deleteTokenFromActiveTokenList(token_t *token)
{
   if((NULL != token) && (NULL != g_activeTokenList)) {

      token_t *prevToken = NULL;

      for(token_t *t = g_activeTokenList;NULL != t;t = t->next) {
         if(t == token)
            break;

         prevToken = t;
      }

      if(NULL != prevToken) {
         prevToken->next = token->next;
      } else {
         g_activeTokenList = token->next;
      }
   }
}

/**
* @fn static token_t *getTokenFromActiveTokenList(uint8_t *payload)
* @brief Eliminacion de token en la lista de tokens a la espera de ser procesados
* @param payload : Puntero a datos del token que se busca
* @return token_t Puntero al token en la lista si este se encontraba en ella, NULL caso contrario
*/
static token_t *getTokenFromActiveTokenList(uint8_t *payload)
{
   if(NULL != payload) {

      for(token_t *t = g_activeTokenList;NULL != t;t = t->next) {
         if(t->payload == payload) {
            return t;
         }
      }
   }

   return NULL;
}

/* State machine action functions */
/** 
* @fn static void registerTimeAction(void *arg)
* @brief Funcion de accion de la maquina de estados en las transiciones
* @param arg : Argumento que es pasado por la funcion que avanza la maquina de estados
* @return Nada
*/
static void registerTimeAction(void *arg) {
   uint32_t *timeUs = (uint32_t *) arg;
   uint64_t _timeUs;

  /* Get timestamp */
  if(timerService_readHwCounterValue(&_timeUs)) {
     *timeUs = (uint32_t) _timeUs;
  }
}

/**
* @fn static void arrivalTime_Cb(uint8_t *payload)
* @brief Handler de obtencion de tiempo de llegada de primer byte
* @param payload : Puntero a los datos del token
* @return Nada
*/
static void arrivalTime_Cb(uint8_t *payload) {

   (void) payload;

   if(NULL != g_currentPassiveToken) {

      fsm(&g_currentPassiveToken->state, FIRST_BYTE_ARRIVAL_EVENT, &g_currentPassiveToken->arrivalTime);
   }
}

/**
* @fn static void receptionTime_Cb(uint8_t *payload)
* @brief Handler de obtencion de tiempo de llegada de ultimo byte
* @param payload : Puntero a los datos del token
* @return Nada
*/
static void receptionTime_Cb(uint8_t *payload) {

   (void) payload;

   if(NULL != g_currentPassiveToken) {

      fsm(&g_currentPassiveToken->state, LAST_BYTE_ARRIVAL_EVENT, &g_currentPassiveToken->receptionTime);
   }
}

/**
* @fn static void exitTime_Cb(uint8_t *payload)
* @brief Handler de obtencion de tiempo de salida de primer byte
* @param payload : Puntero a los datos del token
* @return Nada
*/
static void exitTime_Cb(uint8_t *payload) {
   if(NULL != payload) {
      /* Find active token */
      token_t *activeToken = getTokenFromActiveTokenList(payload);

      if(NULL != activeToken) {
         fsm(&activeToken->state, FIRST_BYTE_SENT_EVENT, &activeToken->exitTime);

      }
   }
}

/**
* @fn static void transmissionTime_Cb(uint8_t *payload)
* @brief Handler de obtencion de tiempo de salida de ultimo byte
* @param payload : Puntero a los datos del token
* @return Nada
*/
static void transmissionTime_Cb(uint8_t *payload) {
   if(NULL != payload) {
      /* Find active token */
      token_t *activeToken = getTokenFromActiveTokenList(payload);

      if(NULL != activeToken) {

         fsm(&activeToken->state, LAST_BYTE_SENT_EVENT, &activeToken->transmissionTime);

         /* Generate token information frame and insert it into transmission queue with memory allocation */
         uint8_t strSize = (uint8_t) snprintf((char *) g_performanceMsgBuf,
                                       sizeof(g_performanceMsgBuf),
                                       "Id:%u, Llegada:%u us, Recepcion:%u us, Inicio:%u us, Fin:%u us, Salida:%u us, Transmision:%u us, Largo Paquete:%u bytes, Memoria Alocada:%u bytes",
                                       (unsigned int) activeToken->packetId,
                                       (unsigned int) activeToken->arrivalTime,
                                       (unsigned int) activeToken->receptionTime,
                                       (unsigned int) activeToken->initialTime,
                                       (unsigned int) activeToken->endTime,
                                       (unsigned int) activeToken->exitTime,
                                       (unsigned int) activeToken->transmissionTime,
                                       (unsigned int) activeToken->frameSize,
                                       (unsigned int) activeToken->usedMemorySize);

         practica3Frame_t frame = {
            .cmdId = PRACTICA_3_CMD_ID_TO_MEAS_PERFORMANCE
         ,  .minimumStackSize = uxTaskGetStackHighWaterMark(NULL)   /* NULL to query its own stack */
         ,  .frameSize = strSize
         ,  .frameData = g_performanceMsgBuf
         };

         practica3Commands_dataPush(&frame, TRUE);

         /* Delete token from activeTokenList */
         deleteTokenFromActiveTokenList(activeToken);

         /* Free token data memory */
         QMPool_put(&g_memPoolHanlder, (void *) activeToken);
      }
   }
}

/**
* @fn static bool_t rxQueuePop(practica2Frame_t *frame) 
* @brief Pop de un frame de la cola de recepcion del comando
* @param frame : Frame a obtener de la cola
* @return bool_t TRUE si se recibio correctamente, FALSE caso contrario
*/
static bool_t rxQueuePop(practica3Frame_t *frame) {

   bool_t ret = FALSE;

   if((NULL != g_rxQueue_Handler) && (NULL != frame)) {

      /* Get data to process */
      if(pdPASS == xQueueReceive(g_rxQueue_Handler, frame, portMAX_DELAY)) {

         ret = TRUE;
      }
   }

   return ret;
}

/**
* @fn static void toMeasPerformance_Task(void *params)
* @brief Tarea de ejecucion del procesador del comando "To Meas Performance"
* @return Nada
* @note Tarea de FreeRTOS
*/
static void toMeasPerformance_Task(void *params) {

  #define TO_MAYUS_CONSTANT    ('A' - 'a')
  practica3Frame_t frame;
  uint8_t *p2Data = NULL;
  uint8_t localSize = 0;

  while(TRUE) {

    if(TRUE == rxQueuePop(&frame)) {

       /* Find active token */
       token_t *activeToken = getTokenFromActiveTokenList(frame.frameData);

       if(NULL != activeToken) {

          /* Advance state machine */
          fsm(&activeToken->state, FRAME_POP_EVENT, &activeToken->initialTime);

          p2Data = frame.frameData;
          localSize = frame.frameSize;

          if((NULL != p2Data) && (0 != localSize)) {

             while(localSize) {

                uint8_t ch = *p2Data;

                if( (ch >= 'a') && (ch <= 'z') )
                   (*p2Data) = (ch + TO_MAYUS_CONSTANT);

                p2Data++;
                --localSize;
             }

             frame.minimumStackSize = uxTaskGetStackHighWaterMark(NULL);   /* NULL to query its own stack */

             /* Save total allocated space for current packet */
             activeToken->frameSize = frame.frameSize + sizeof(token_t);

             /* Save memory block size for current packet */
             activeToken->usedMemorySize = practica3Commands_getMemPoolBlockSize(&frame) + g_memPoolHanlder.blockSize;

             practica3Commands_dataPush(&frame, FALSE);

             /* Advance state machine */
             fsm(&activeToken->state, FRAME_PUSH_EVENT, &activeToken->endTime);
          }
       }
    }
  }
}

/*==================[definiciones de funciones externas]=====================*/

bool_t toMeasPerformanceCommand_init(void) {


   /* Create task to process to minus command */
   if (pdPASS != xTaskCreate(toMeasPerformance_Task,              // Task Function
                             "toMeasPerformance_Task",            // Task Name as string for user
                             2 * configMINIMAL_STACK_SIZE,        // Task Stack size
                             NULL,                                // Task Parameters
                             tskIDLE_PRIORITY + 2,                // Task Priority
                             NULL                                 // Task Handler
                            ))
      return FALSE;

   /* Create Rx queue */

   g_rxQueue_Handler = xQueueCreate(QUEUE_ENTRIES_MAX, sizeof(token_t));

   if(NULL == g_rxQueue_Handler)
      return FALSE;

   /* Create memory pool to allocate tokens */
   QMPool_init(&g_memPoolHanlder,
               &g_memPool,
               sizeof(g_memPool),
               sizeof(token_t));

   /* Allocate current passive token */
   g_currentPassiveToken = (token_t *) QMPool_get(&g_memPoolHanlder, 0U);

   if(NULL == g_currentPassiveToken)
      return FALSE;

   memset(g_currentPassiveToken, 0, sizeof(token_t));

   /* Set token initial state */
   g_currentPassiveToken->state = (fsmState_t *) waitingFirstByteArrival;

   /* Register observer callbacks (in ACT model means registering callbacks into server) */
   if(FALSE == practica3Commands_registerIndHandlers(TO_MEAS_PERFORMANCE_TASK_ID, &g_observerCbs))
      return FALSE;

   /* Initialize hardware timer */
   if(FALSE == timerService_initHw())
      return FALSE;

   return TRUE;
}

void toMeasPerformanceCommand_dataPush(const practica3Frame_t *frame) {

  /* Push newFrame into queue */
  if((NULL != g_rxQueue_Handler) && (NULL != frame) && (NULL != g_currentPassiveToken)) {

     /* Set packetId */
     g_currentPassiveToken->packetId = getPacketId();

     /* Assign received frame with currentPassive token */
     g_currentPassiveToken->payload = (uint8_t *) frame->frameData;

     /* Insert current passive token into activeToken list */
     placeTokenIntoActiveTokenList(g_currentPassiveToken);

     if(pdPASS == xQueueSend(g_rxQueue_Handler, frame, 0)) {

        /* Allocate a new active token */
        /* Set to NULL if no more space available, which automatically blocks further processing */
        g_currentPassiveToken = (token_t *) QMPool_get(&g_memPoolHanlder, 0U);

        memset(g_currentPassiveToken, 0, sizeof(token_t));

        /* Set token initial state */
        g_currentPassiveToken->state = (fsmState_t *) waitingFirstByteArrival;
     }
  }
}

/*==================[fin del archivo]========================================*/
