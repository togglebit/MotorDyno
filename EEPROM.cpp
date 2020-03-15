/*
  EEPROM.cpp - EEPROM library
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

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <avr/eeprom.h>

#if  ARDUINO >= 100
//WProgram renamed to Arduino.h. in new IDE 1.5.2
#include "Arduino.h"
#else
#include "WConstants.h"
#endif


#include "EEPROM.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Constructors
 ******************************************************************************/

/******************************************************************************
 * User API
 ******************************************************************************/

uint8_t EEPROMClass::read(int address)
    {
    return eeprom_read_byte((unsigned char *) address);
    }

void EEPROMClass::write(int address, uint8_t value)
    {
    eeprom_write_byte((unsigned char *) address, value);
    }
//this function overloads for reading and writing floats, structs used to avoid type conversion issues with floats
float EEPROMClass::readFloat(int address)
    {

    unsigned char i,x;
    union
        {
        float fl;
        unsigned char b[4];
        }U;

    //byte array index for endianness
    x = sizeof(float)-1;

    for (i=0; i<sizeof(float); i++)
        {
        U.b[i] = eeprom_read_byte ((unsigned char *)(address+x--)); 
        }
    
    return(U.fl);
    }

//this function overloads for reading and writing floats, structs used to avoid type conversion issues with floats
void EEPROMClass::writeFloat(int address, float f)
    {

    unsigned char i,x;
    union
        {
        float fl;
        unsigned char b[4];
        }U;

    //store float away for byte reference via union
    U.fl = f;

    //byte array index for endianness
    x = sizeof(float)-1;

    //write
    for (i=0; i<sizeof(float); i++)
        {
       eeprom_write_byte((unsigned char *)(address+i), U.b[x--]);
        }
    }

EEPROMClass EEPROM;
