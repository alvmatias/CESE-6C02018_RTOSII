/** 
* @file  ring_buffer.h
* @brief Implementacion de buffer circular
* @note Autores:  Alvarez Matias - Santarcangelo Marco - Tirapegui Rodrigo
      Licencia: 
      Fecha: 17/10/2018
*/

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

/*==================[inclusiones]============================================*/
#include "sapi_datatypes.h"
/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/
/**
 * @struct ring_buffer
 * @brief Estructura para contener el buffer circular
 */
struct ring_buffer {
   volatile uint16_t write_offset;  /**< Offset de escritura dentro del buffer circular */ 
   volatile uint16_t read_offset;   /**< Offset de lectura dentro del buffer circular */
   uint16_t size;                   /**< Tamaño del buffer circular */
   uint8_t *buffer;                 /**< Puntero al buffer circular */
};
/*==================[declaraciones de datos internos]========================*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones internas]====================*/


/**
 * @fn static inline uint16_t getNext(uint16_t cur_offset, uint16_t size)
 * @brief Funcion para obtener el offset en un buffer circular
 * @param cur_offset : El offset actual del buffer circular
 * @param size       : El tamano del buffer circular en bytes
 * @return El siguiente offset o 0 en caso de overflow
 */
static inline uint16_t getNext(uint16_t cur_offset, uint16_t size) {
   return (cur_offset == (size - 1) ? 0 : cur_offset + 1);
}

/**
 * @fn static inline uint16_t ringBufferGetNextWrite(const struct ring_buffer *ring)
 * @brief Funcion para obtener el siguiente offset de escritura del buffer circular
 * @param ring : Puntero a una estructura de tipo ring_buffer
 * @return El siguiente offset de escritura o 0 en caso de overflow
 */
static inline uint16_t ringBufferGetNextWrite(const struct ring_buffer *ring) {
   return getNext(ring->write_offset, ring->size);
}

/**
 * @fn static inline uint16_t ringBufferGetNextRead(const struct ring_buffer *ring)
 * @brief Funcion para obtener el siguiente offset de lectura del buffer circular
 * @param ring : Puntero a una estructura de tipo ring_buffer
 * @return El siguiente offset de lectura o 0 en caso de overflow
 */
static inline uint16_t ringBufferGetNextRead(const struct ring_buffer *ring) {
   return getNext(ring->read_offset, ring->size);
}

/**
 * @fn static inline bool_t ringBufferIsFull(const struct ring_buffer *ring) 
 * @brief Funcion para verificar si el buffer circular esta lleno
 * @param ring : Puntero a una estructura de tipo ring_buffer
 * @return bool_t TRUE si el buffer esta lleno, FALSE caso contrario.
 */
static inline bool_t ringBufferIsFull(const struct ring_buffer *ring) {
   return (ring->read_offset == ringBufferGetNextWrite(ring));
}

/**
 * @fn static inline bool_t ringBufferIsEmpty(const struct ring_buffer *ring) 
 * @brief Funcion para verificar si el buffer circular esta lleno
 * @param ring : Puntero a una estructura de tipo ring_buffer
 * @return bool_t TRUE si el buffer esta vacio, FALSE caso contrario.
 */
static inline bool_t ringBufferIsEmpty(const struct ring_buffer *ring) {
   return (ring->read_offset == ring->write_offset);
}

/**
 * @fn static inline struct ring_buffer ringBufferInit(uint8_t *buffer, uint16_t size)
 * @brief Funcion para inicializar el buffer circular
 * @param buffer : Puntero a un buffer circular
 * @param size   : Tamaño del buffer circular
 * @return struct ring_buffer : Estructura que contiene el buffer circular
 */
static inline struct ring_buffer ringBufferInit(uint8_t *buffer, uint16_t size) {
   struct ring_buffer ring;
   ring.write_offset = 0;
   ring.read_offset = 0;
   ring.size = size;
   ring.buffer = buffer;
   return ring;
}

/**
 * @fn static inline uint8_t ringBufferGet(struct ring_buffer *ring)
 * @brief Funcion para obtener un byte de un buffer circular
 * @param ring : Puntero a una estructura de tipo ring_buffer
 * @return El siguiente byte del buffer
 * @note Llamese a esta funcion para obtener un byte del buffer circular.
         Asegurese que el buffer no esta vacio usando
         (@ref ringBufferIsEmpty) antes de invocar esta funcion
 */
static inline uint8_t ringBufferGet(struct ring_buffer *ring) {
   uint8_t data = ring->buffer[ring->read_offset];
   ring->read_offset = ringBufferGetNextRead(ring);
   return data;
}

/**
 * @fn static inline void ringBufferPut(struct ring_buffer *ring, uint8_t data)
 * @brief Funcion para poner un byte en el buffer circular
 * @param ring : Puntero a una estructura de tipo ring_buffer
 * @param data : El byte a insertar en el buffer
 * @return Nada.
 * @note Llamese a esta funcion para insertar un byte en el buffer circular.
         Asegurese que el buffer no esta lleno usando
         (@ref ringBufferIsFull) antes de invocar esta funcion
 */
static inline void ringBufferPut(struct ring_buffer *ring, uint8_t data) {
   ring->buffer[ring->write_offset] = data;
   ring->write_offset = ringBufferGetNextWrite(ring);
}
/*==================[declaraciones de funciones externas]====================*/

/*==================[end of file]============================================*/

#endif /* _RING_BUFFER_H_ */