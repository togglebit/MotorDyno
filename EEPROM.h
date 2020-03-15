/*
  EEPROM.h - EEPROM library
  Copyright (c) 2006 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EEPROM_h
#define EEPROM_h

#include <inttypes.h>

class EEPROMClass  
    {
public:
    uint8_t read(int);
    float readFloat(int);
    void write(int, uint8_t);
    void writeFloat(int address, float f);

    };

extern EEPROMClass EEPROM;


#define READ_EE_U16(addr) (EEPROM.read(addr) << 8 | (EEPROM.read(addr+1)))
#define WRITE_EE_U16(addr,value) EEPROM.write(addr,(UINT8)((value & 0xFF00) >> 8 )); EEPROM.write(addr+1,(UINT8)(value & 0x00FF)) 

#define READ_EE_FLOAT(addr) (float)((EEPROM.read(addr) << 24) | (EEPROM.read(addr+1) << 16) |  (EEPROM.read(addr+2) << 8) | (EEPROM.read(addr+3)));

#define WRITE_EE_FLOAT(addr,ptr)    EEPROM.write(addr+3,  ptr++); \ 
                                    EEPROM.write(addr+2,  ptr++); \
                                    EEPROM.write(addr+1,  ptr++); \
                                    EEPROM.write(addr,    ptr++)


#define PROTO_BASE 0x0000
#define DEV_BASE 0x0080

//************ EEPROM MEMORY MAP *******************
// PROTOCOL VARIABLES
#define EEPROM_TIME_U16         PROTO_BASE 
#define EEPROM_THRESH_FLOAT     PROTO_BASE + sizeof(UINT16)
#define EEPROM_ERRTHRESH_FLOAT  EEPROM_THRESH_FLOAT + sizeof(float)
#define EEPROM_DUTY_UINT8       EEPROM_ERRTHRESH_FLOAT + sizeof(float)
#define EEPROM_PATTERN_UINT16   EEPROM_DUTY_UINT8 + sizeof(UINT16)
#define EEPROM_REARMTIME_U16    EEPROM_PATTERN_UINT16 + sizeof(UINT16)
// DEVICE VARIABLES 
#define EEPROM_NODEID 			DEV_BASE
#define EEPROM_SERIAL 			DEV_BASE + sizeof(UINT16)
#define EEPROM_FIRMWARE 		EEPROM_SERIAL + sizeof(UINT16)
#endif

