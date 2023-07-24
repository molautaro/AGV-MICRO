/*
 * Util.h
 *
 *  Created on: 22 sep. 2021
 *      Author: Guillermo
 */

#ifndef UTIL_H_
#define UTIL_H_

//acá escribir las estructuras y tipos común a todo el proyecto
typedef struct {
	uint8_t *buf;
	uint8_t iW;
	uint8_t iR;
	uint8_t header;
	uint16_t timeout;
	uint8_t nBytes;
	uint8_t iData;
	uint8_t cks;
}__attribute__((packed, aligned(1))) _rx;

typedef struct {
	uint8_t *buf;
	uint8_t iW;
	uint8_t iR;
	uint8_t cks;
}__attribute__((packed, aligned(1))) _tx;


#endif /* UTIL_H_ */
