/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RF24Network_config.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "nodeconfig.h"

// Avoid spurious warnings
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

// Where in EEPROM is the address stored?
uint8_t* address_at_eeprom_location = (uint8_t*)10;

// What flag value is stored there so we know the value is valid?
const uint8_t valid_eeprom_flag = 0xdf;

uint8_t nodeconfig_read(void)
{
  uint8_t result = 0;

  // Look for the token in EEPROM to indicate the following value is
  // a validly set node address 
  if ( eeprom_read_byte(address_at_eeprom_location) == valid_eeprom_flag )
  {
    // Read the address from EEPROM
    result = eeprom_read_byte(address_at_eeprom_location+1);
    printf_P(PSTR("ADDRESS: %u\n\r"),result);
  }
  else
  {
    printf_P(PSTR("*** No valid address found.  Send 0-9 via serial to set node address\n\r"));
    while(1)
    {
      nodeconfig_listen();
    }
  }
  
  return result;
}

void nodeconfig_listen(void)
{
  //
  // Listen for serial input, which is how we set the address
  //
  if (Serial.available())
  {
    // If the character on serial input is in a valid range...
    char c = Serial.read();
    if ( c >= '0' && c <= '9' )
    {
      // It is our address
      eeprom_write_byte(address_at_eeprom_location,valid_eeprom_flag);
      eeprom_write_byte(address_at_eeprom_location+1,c-'0');

      // And we are done right now (no easy way to soft reset)
      printf_P(PSTR("\n\rManually reset address to: %c\n\rPress RESET to continue!"),c);
      while(1) ;
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
