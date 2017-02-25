/*
 * eeprom_mmap.h
 *
 *  Created on: 2016��6��30��
 *      Author: zctiaxia
 */

#ifndef SRC_EEPROM_MMAP_H_
#define SRC_EEPROM_MMAP_H_

#define MEMDEP_MCB 			13
#define MEMDEP_XYZ 			27
#define MEMDEP_LIGHTCTRL	13
#define MEMDEP_RFLM			50
#define	MEMDEP_TMLM			14
#define MEMDEP_USRSETTING	31


#define MEMDEP_ELEMENTS_NP 	102 
#define MEMDEP_ELEMENTS_REF 51

#define	WRONGVALUE	NULL

#define ADDR_ELEMENTS_REF       3840


extern const uint8_t MMAP_MCB[][];
extern const uint8_t MMAP_XYZ[][];
extern const uint8_t MMAP_LIGHTCTRL[][];
extern const uint8_t MMAP_RFLM[][];
extern const uint8_t MMAP_TMLM[][];
extern const uint8_t MMAP_USRSETTING[][];
extern const uint8_t MMAP_ELEMENTS_NP[][];
extern const uint8_t MMAP_ELEMENTS_REF[][];


extern const Uint16* BinarySearch(const Uint16 arr[][3],Uint16 len,Uint16 key);

#endif /* SRC_EEPROM_MMAP_H_ */
