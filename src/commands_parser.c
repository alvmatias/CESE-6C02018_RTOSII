/**
* @file  commands_parser.c
* @brief Implementacion del parser de los paquetes.
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
		Licencia:
		Fecha: 17/10/2018
*/

/*==================[inlcusiones]============================================*/
#include <string.h>
#include "commands_parser.h"
#include "preprocessor.h"
#include "qmpool.h"
#include "uart_usb.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/*==================[definiciones y macros]==================================*/

/**
* @def BUF_SIZE_MAX
* @brief Tamaño maximo de los buffers internos
*/
#define BUF_SIZE_MAX   10

/* Memory pool size definitions */
/**
* @def MEM_POOL_COUNT
* @brief Cantidad de mem pools
* @note Decimal > 0
*/
#define MEM_POOL_COUNT              3                             // Decimal > 0

/**
* @def MEM_POOL_TYPE_X_SYZE
* @brief Tamaño de cada mem pool
* @note En bytes
*/
#define MEM_POOL_TYPE_X_SIZE        512

#if (0 == (MEM_POOL_TYPE_X_SIZE >> MEM_POOL_COUNT))
#  error "Either pool size should be defined bigger or pool count should be defined smaller!"
#endif

/**
* @def INIT_MEM_POOL(n, unused)
* @brief Macro para inicializacion de un mem pool
*/
#define INIT_MEM_POOL(n, unused)                         \
         QMPool_init(&g_memPoolT##n##_Handler,           \
                     &g_memPool[MEM_POOL_TYPE_X_SIZE*n], \
                     MEM_POOL_TYPE_X_SIZE,               \
                     MEM_POOL_TYPE_##n##_BLOCK_SIZE);    \

/**
* @def DECLARE_MEM_POOL_BLOCK_SIZE(n, unused)
* @brief Macro para declarar el tamaño de bloque del mem pool de acuerdo al tipo
*/
#define DECLARE_MEM_POOL_BLOCK_SIZE(n, unused)                    \
   MEM_POOL_TYPE_##n##_BLOCK_SIZE = (MEM_POOL_TYPE_X_SIZE >> n),  \

/**
* @def INSTANCIATE_MEM_POOL_HANLDER(n, unused)
* @brief Macro para instancia el mem pool de acuerdo al tipo
*/
#define INSTANCIATE_MEM_POOL_HANLDER(n, unused)  \
   static QMPool g_memPoolT##n##_Handler;

/**
* @def GET_OPTIMAL_MEM_POOL_BY_DATA_SIZE(n, unsused)
* @brief Macro para obtener mem pool de tamaño optimo de acuerdo al tamaño del frame
*/
#define GET_OPTIMAL_MEM_POOL_BY_DATA_SIZE(n, unsused)          \
      if(MEM_POOL_TYPE_##n##_BLOCK_SIZE > rcvDataSize) {       \
         memPoolHandler = &g_memPoolT##n##_Handler;            \
      }

/**
* @def GET_MEM_POOL_HANDLER_BY_DATA_ADDRESS(n, unsused)
* @brief Macro para obtener el manejador del mem pool de acuerdo a la direccion del puntero a datos
*/
#define GET_MEM_POOL_HANDLER_BY_DATA_ADDRESS(n, unsused)             \
      if((&g_memPool[MEM_POOL_TYPE_X_SIZE*n] <= sndData) &&        \
         (&g_memPool[MEM_POOL_TYPE_X_SIZE*(n + 1)] > sndData)) {   \
         memPoolHandler = &g_memPoolT##n##_Handler;                  \
      }

/**
* @enum memPool_blockSize
* @brief Definicion auto-completada de los handlers de los mem pools
* @attention NO MODIFICAR
*/
enum memPool_blockSize {
      MREPEAT(MEM_POOL_COUNT, DECLARE_MEM_POOL_BLOCK_SIZE, ~)
};

/*
 * Serial protocol:
 *
 * (1 byte)   (1 byte)       (1 byte)       (pl bytes)  (1 byte)
 *
 *   STX   +   cmdId    +  payloadLength  +  payload  +  ETX
 */

/* Serial protocol control characters */
/**
* @def STX
* @brief Delimitador de comienzo de paquete
*/
#define STX 0x55

/**
* @def ETX
* @brief Delimitador de fin de paquete
*/
#define ETX 0xAA

/**
* @def CMDS_UART_BUF_SIZE_MAX
* @brief Tamaño maximo del campo datos del paquete
*/
#define CMDS_UART_BUF_SIZE_MAX      255

/**
* @def CMDS_UART_RCV_DATA_TIMEOUT
* @brief Timeout de la recepcion de datos
* @note En milisegundos
*/
#define CMDS_UART_RCV_DATA_TIMEOUT   100

/**
* @def TASKS_ID_MAX
* @brief Maxima cantidad de tareas en la aplicacion
*/
#define TASKS_ID_MAX   3

/**
* @enum cmds_rcv_state_t
* @brief Estados posibles de la maquina de estados de recepcion del protocolo serie
*/
typedef enum
{
  CMDS_RCV_IDLE            = 0x00
, CMDS_RCV_WAITING_CMD_ID
, CMDS_RCV_WAITING_EXP_LEN
, CMDS_RCV_WAITING_PAYLOAD_AND_ETX
} cmds_rcv_state_t;

/**
* @enum observerCb
* @brief Tipo del callback de medicion de performance
*/
enum observerCb {
   CB_RCV_STX
,  CB_RCV_ETX
,  CB_SND_STX
,  CB_SND_ETX
};

/**
* @struct taskObserver_data_t
* @brief Datos de las tareas de observacion
* @note Agregar mas campos debajo si se necesitan
*/
typedef struct {
   const commands_cbHandlers_t *cbs;
} taskObserver_data_t;

/*==================[definiciones de datos internos]=========================*/

/**
* @var g_rcvCmdId
* @brief Id de comando recibido
*/
static uint8_t g_rcvCmdId;

/**
* @var g_sndCmdId
* @brief Id de comando a enviar
*/
static uint8_t g_sndCmdId;

/**
* @var g_rcvDataSize
* @brief Tamaño del campo de datos recibido
*/
static uint8_t g_rcvDataSize;

/**
* @var g_minumimStackSize
* @brief Minimo stack disponible del procesador de comandos a enviar
*/
static uint32_t g_minumimStackSize;

/**
* @var g_sndDataSize
* @brief Tamaño del campo de datos a enviar
*/
static uint8_t g_sndDataSize;

/**
* @var g_sndData
* @brief Puntero a los datos a enviar
*/
static uint8_t *g_sndData;

/**
* @var g_rcvData[CMDS_UART_BUF_SIZE_MAX]
* @brief Buffer de los datos a enviar
*/
static uint8_t g_rcvData[CMDS_UART_BUF_SIZE_MAX];


/* Memory pool space */
/**
* @var g_memPool[MEM_POOL_COUNT * MEM_POOL_TYPE_X_SIZE]
* @brief Espacio de memoria reservado para los memory pools
*/
static uint8_t g_memPool[MEM_POOL_COUNT * MEM_POOL_TYPE_X_SIZE];

/**
* @var static QueueHandle_t g_txQueue_Handler
* @brief Hanlder de la cola de transmision
*/
static QueueHandle_t g_txQueue_Handler = NULL;

/**
* @var static taskObserver_data_t g_taskObserverCallbacks[TASKS_ID_MAX]
* @brief Callback Handlers de la aplicacion para la medicion de performance
*/
static taskObserver_data_t g_taskObserverCallbacks[TASKS_ID_MAX];

/* Auto-intanciate of memory pools handlers (do not change!) */
MREPEAT(MEM_POOL_COUNT, INSTANCIATE_MEM_POOL_HANLDER, ~);

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/
/**
* @fn static void invokeObserversCallbacks(enum observerCb cbType, uint8_t *frame)
* @brief Invocacion de los callbacks de medicion de performance
* @param cbType : Tipo de callback
* @param frame  : Puntero al frame conteniendo los datos
* @return Nada
*/
static void invokeObserversCallbacks(enum observerCb cbType, uint8_t *frame) {

   uint8_t i;
   asyncOp_cbHandler_t p2Func;

   for(i = 0, p2Func = NULL;i < TASKS_ID_MAX;i++) {

      if(NULL != g_taskObserverCallbacks[i].cbs) {
         switch(cbType) {
            case CB_RCV_STX:  p2Func = g_taskObserverCallbacks[i].cbs->rcvSTX_cbHandler; break;
            case CB_RCV_ETX:  p2Func = g_taskObserverCallbacks[i].cbs->rcvETX_cbHandler; break;
            case CB_SND_STX:  p2Func = g_taskObserverCallbacks[i].cbs->sndSTX_cbHandler; break;
            case CB_SND_ETX:  p2Func = g_taskObserverCallbacks[i].cbs->sndETX_cbHandler; break;
            default:          return;
         }

         if(NULL != p2Func)
            (p2Func) (frame);
      }
   }
}

/**
* @fn static void processUARTCompleteMsg(void)
* @brief Envio de mensaje recibido por UART al procesador de comando respectivo
* @param Ninguno
* @return Nada
*/
static void processUARTCompleteMsg(void) {

   uint8_t i;

   /* Check if cmdId processor handler exists and invoke function callback */
   for(i = 0;COMMANDS_PARSER_INVALID_CMD_ID != g_commandsArr[i].cmdId;i++) {

      if(g_rcvCmdId == g_commandsArr[i].cmdId) {

         if(NULL != g_commandsArr[i].pfnDataPush) {

            /* Check if received data length is < MEM_POOL_TYPE_X_SIZE */
            if(MEM_POOL_TYPE_X_SIZE > g_rcvDataSize) {

               QMPool *memPoolHandler = NULL;
               uint8_t rcvDataSize = g_rcvDataSize;

               frame_t newFrame = {
                                    .cmdId = (uint8_t) g_rcvCmdId
                                  , .frameSize = (uint8_t) g_rcvDataSize
                                  , .frameData = NULL
                                 };

               /* Get memory pool handler (Do not change!) */
               MREPEAT(MEM_POOL_COUNT, GET_OPTIMAL_MEM_POOL_BY_DATA_SIZE, ~);

               if(NULL != memPoolHandler) {

                  newFrame.frameData = (uint8_t *) QMPool_get(memPoolHandler, 0U);

                  if(NULL != newFrame.frameData) {

                     /* Copy frame data to memory pool block */
                     strncpy((char *) newFrame.frameData, (const char *) g_rcvData, g_rcvDataSize);

                     /* Push newFrame into queue */
                     g_commandsArr[i].pfnDataPush(&newFrame);
                  }
               }
            }
         }
      }
   }
}

/**
* @fn static bool_t getUARTCompleteMsg(uint32_t invokePeriodMs)
* @brief Recepcion de paquete via UART - Implementado con maquina de estados
* @param invokePeriodMs : Periodicidad de invocacion de la funcion
* @return bool_t  TRUE si la data recibida es correcta,
                  FALSE si no se recibio correctamente o todavia no se recibieron toos los datos.
*/
static bool_t getUARTCompleteMsg(uint32_t invokePeriodMs) {

   static cmds_rcv_state_t rcvState = CMDS_RCV_IDLE;
   static uint8_t          _payldByteCnt, timeout = 0;     // Number of times is call before received data is discarted
   uint8_t                 byte, expTimeout;
   bool_t                  keepGoing = TRUE, ret = FALSE;

   expTimeout = (CMDS_UART_RCV_DATA_TIMEOUT/invokePeriodMs) + 1;

   /* Reset state when timeout */
   if(0 == (timeout && --timeout))
      rcvState = CMDS_RCV_IDLE;

   /* Advance state machine at maximum speed until there's no more data
    * inside UART buffer.
    * Consider that mostly UART byte transfers are slower that CPU speed,
    * so maybe one full message isn't available at one time.
    * */
   while(keepGoing) {

      /* Get byte from uart */
      if(TRUE == (keepGoing = uartUsbReadByte(&byte))) {

         switch(rcvState) {

            case CMDS_RCV_IDLE:
            {
               if(STX == byte) {
                  /* Kick timeout */
                  timeout  = expTimeout;
                  rcvState = CMDS_RCV_WAITING_CMD_ID;

                  /* Invoke observers callback */
                  invokeObserversCallbacks(CB_RCV_STX, NULL);
               }
            } break;
            case CMDS_RCV_WAITING_CMD_ID:
            {
               /* Kick timeout */
               timeout  = expTimeout;
               rcvState = CMDS_RCV_WAITING_EXP_LEN;

               /* Save message cmdId */
               g_rcvCmdId   = byte;
            } break;
            case CMDS_RCV_WAITING_EXP_LEN:
            {
               if(0 != byte) {
                  /* Kick timeout */
                  timeout  = expTimeout;
                  rcvState = CMDS_RCV_WAITING_PAYLOAD_AND_ETX;

                  /* Save message payload's expected length */
                  g_rcvDataSize = byte;

                  /* Reset payload data byte counter */
                  _payldByteCnt = 0;
               } else {
                  /* If expected length is 0, go to CMDS_RCV_IDLE */
                  rcvState = CMDS_RCV_IDLE;
               }
            } break;
            case CMDS_RCV_WAITING_PAYLOAD_AND_ETX:
            {
               /* Kick timeout */
               timeout = expTimeout;

               if(_payldByteCnt < g_rcvDataSize) {
                  /* Save current payload byte in buffer */
                  g_rcvData[_payldByteCnt++] = byte;
               } else {
                  /* Now should come the ETX byte */
                  if(ETX == byte) {
                     /* Cancel receive data timeout timer and return message as completed */
                     timeout = 0;
                     ret     = TRUE;

                     /* Invoke observers callback */
                     invokeObserversCallbacks(CB_RCV_ETX, NULL);
                  }

                  /* Reset received state to idle */
                  rcvState = CMDS_RCV_IDLE;

                  /* Get out from while loop */
                  keepGoing = FALSE;
               }
            } break;

            default:
            {
               rcvState = CMDS_RCV_IDLE;
            } break;
         }
      }
   }

  return ret;
}

/**
* @fn static void processTASKSCompleteMsg(void)
* @brief Formateo y envio de paquete via UART
* @params Ninguno
* @return Nada
*/
void processTASKSCompleteMsg(void) {

   /* Format message and send it through UART. Then free memory. */

   if ((COMMANDS_PARSER_INVALID_CMD_ID != g_sndCmdId) && (0 < g_sndDataSize)) {

      uint8_t strBuf[BUF_SIZE_MAX];
      uint8_t strSize = 0;

      /* Send formatted message */
      uartUsbWriteByte(STX);

      /* Invoke observers callback */
      invokeObserversCallbacks(CB_SND_STX, g_sndData);

      uartUsbWriteByte(g_sndCmdId);
      uartUsbWriteByte(g_sndDataSize);
      uartUsbWriteData(g_sndData, g_sndDataSize);
      uartUsbWriteByte(ETX);

      /* Invoke observers callback */
      invokeObserversCallbacks(CB_SND_ETX, g_sndData);

      /* Send minimum stack size */
      strSize = (uint8_t) snprintf((char *) strBuf, sizeof(strBuf), "%u", (unsigned int) g_minumimStackSize);

      uartUsbWriteByte(STX);
      uartUsbWriteByte(COMMANDS_PARSER_MINIMUM_STACK_CMD_ID);
      uartUsbWriteByte(strSize);
      uartUsbWriteData(strBuf, strSize);
      uartUsbWriteByte(ETX);

      /* Free sent data memory pool */
      QMPool *memPoolHandler = NULL;
      uint8_t *sndData = g_sndData;

      /* Get memory pool handler */
      MREPEAT(MEM_POOL_COUNT, GET_MEM_POOL_HANDLER_BY_DATA_ADDRESS, ~);

      if(NULL != memPoolHandler) {

         /* Free allocated memory */
         QMPool_put(memPoolHandler, (void *) g_sndData);
      }
   }
}

/**
* @fn static bool_t getTASKSCompleteMsg(void)
* @brief Obtencion de datos procesados del procesador de cada comando
* @params Ninguno
* @return bool_t TRUE si la obtencion de los datos fue correcta , FALSE caso contrario.
*/
bool_t getTASKSCompleteMsg(void) {

   /* Initialize global variables */
   g_sndCmdId = COMMANDS_PARSER_INVALID_CMD_ID;
   g_minumimStackSize = 0;
   g_sndDataSize = 0;
   g_sndData = NULL;

   /* Check if any tasks has data to be sent through UART interface.
    * Once data is get, return and wait to be called again */
   if(NULL != g_txQueue_Handler) {
      frame_t newFrame;

      /* Get data to process */
      if(pdPASS == xQueueReceive(g_txQueue_Handler, &newFrame, 0)) {

         g_sndCmdId = newFrame.cmdId;
         g_minumimStackSize = newFrame.minimumStackSize;
         g_sndDataSize = newFrame.frameSize;
         g_sndData = newFrame.frameData;

         return TRUE;
      }
   }

   return FALSE;
}

/**
* @fn static void commandsParser_Task(void *params)
* @brief Tarea de ejecucion del parser
* @return Nada
* @note Tarea de FreeRTOS
*/
static void commandsParser_Task(void *params) {
   
   #define SLEEP_INTERVAL  1       // In milliseconds

   while(TRUE) {

      /* Command reception logic */
      if(getUARTCompleteMsg(SLEEP_INTERVAL))
         processUARTCompleteMsg();

      /* Command send logic */
      if(getTASKSCompleteMsg())
         processTASKSCompleteMsg();

      vTaskDelay(SLEEP_INTERVAL / portTICK_RATE_MS);
   }
}

/*==================[definiciones de funciones externas]=====================*/

bool_t commandsParser_init(void) {
   
   uint8_t strBuf[BUF_SIZE_MAX];
   uint8_t strSize = 0;

   /* Create task to parse incoming messages from UART */
   if(pdPASS != xTaskCreate(commandsParser_Task,                  // Task Function
                            "commandsParser_Task",                // Task Name as string for user
                            2 * configMINIMAL_STACK_SIZE,         // Task Stack size
                            NULL,                                 // Task Parameters
                            tskIDLE_PRIORITY + 1,                 // Task Priority
                            NULL                                  // Task Handler
                            ))
      return FALSE;

   /* Auto-initialize memory pools (Do not change!) */
   MREPEAT(MEM_POOL_COUNT, INIT_MEM_POOL, ~);
   
   /* Create queue transimission */
   g_txQueue_Handler = xQueueCreate(COMMANDS_PARSER_TX_QUEUE_ENTRIES_MAX, sizeof(frame_t));

   if(NULL == g_txQueue_Handler)
      return FALSE;

   /* Initialize UART driver */  // @todo.rtirapegui By now UART driver is not reentrant.
   uartUsbInit();

   /* Send available heap data */
   strSize = (uint8_t) snprintf((char *) strBuf, sizeof(strBuf), "%u", sizeof(g_memPool));

   uartUsbWriteByte(STX);
   uartUsbWriteByte(COMMANDS_PARSER_AVAILABLE_HEAP_CMD_ID);
   uartUsbWriteByte(strSize);
   uartUsbWriteData(strBuf, strSize);
   uartUsbWriteByte(ETX);

   return TRUE;
}

bool_t commandsParser_registerIndHandlers(const uint8_t taskId, const commands_cbHandlers_t * observerCbHandlers) {

   if((TASKS_ID_MAX > taskId) && (NULL != observerCbHandlers)) {

      g_taskObserverCallbacks[taskId].cbs = observerCbHandlers;

      return TRUE;
   }

   return FALSE;
}

uint32_t commandsParser_getMemPoolBlockSize(const frame_t *frame) {

   if(NULL != frame) {

      QMPool *memPoolHandler = NULL;
      uint8_t *sndData = frame->frameData;

      /* Get memory pool handler */
      MREPEAT(MEM_POOL_COUNT, GET_MEM_POOL_HANDLER_BY_DATA_ADDRESS, ~);

      if(NULL != memPoolHandler) {
         return memPoolHandler->blockSize;
      }
   }

   return 0;
}

bool_t commandsParser_dataPush(frame_t *frame, bool_t withAllocMem) {

   if((NULL != g_txQueue_Handler) && (NULL != frame)) {

      if(TRUE == withAllocMem) {
         QMPool *memPoolHandler = NULL;
         uint8_t *p2Memory = NULL;
         uint8_t rcvDataSize = frame->frameSize;

         /* Get memory pool handler (Do not change!) */
         MREPEAT(MEM_POOL_COUNT, GET_OPTIMAL_MEM_POOL_BY_DATA_SIZE, ~);

         if(NULL != memPoolHandler) {

            p2Memory = (uint8_t *) QMPool_get(memPoolHandler, 0U);

            if(NULL != p2Memory) {

               /* Copy frame data to memory pool block */
               strncpy((char *) p2Memory, (const char *) frame->frameData, rcvDataSize);

               /* Save memory pointer into frame */
               frame->frameData = p2Memory;
            } else {
               return FALSE;
            }
         }
      }

      return (pdTRUE == xQueueSend(g_txQueue_Handler, frame, 0));
  }

  return FALSE;
}

/*==================[fin del archivo]========================================*/
