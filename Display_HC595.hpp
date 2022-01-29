
#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "defs.hpp"

class Display_HC595
{
private:
   const uint8_t* cathodes{nullptr};
   const uint8_t* digits{nullptr};
   uint8_t memory[ DISPLAY_HC595_CATHODES ];

public:
   Display_HC595();
   Display_HC595(Display_HC595&) = delete;
   Display_HC595& operator=(Display_HC595&) = delete;

   void clear();
   void print_str( uint8_t* str );
   void print_number( uint16_t num, uint8_t dp_pos, bool leading_zero = false );

   void begin( const uint8_t* cathodes, const uint8_t* digits );
   void update();
};

