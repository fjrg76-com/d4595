/*Copyright (C) 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * 2022 - fjrg76 at hotmail dot com
 */

#include <stdint.h>
#include <string.h>

#include "defs.hpp"
#include "Display_HC595.hpp"

#define HUNDREDTHS_COUNTER_MOD 100
#define SECONDS_COUNTER_MOD 15

#define SCROLL_PERIOD 3 // x100 [ms]

enum class eMode: uint8_t
{ 
   NONE, HUNDREDTHS, SECONDS, TEXT 
};


constexpr uint8_t digits_array[18] =
{
   SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F, // 0
   SEG_B + SEG_C,
   SEG_A + SEG_B + SEG_G + SEG_E + SEG_D,
   SEG_A + SEG_B + SEG_G + SEG_C + SEG_D,
   SEG_F + SEG_G + SEG_B + SEG_C,
   SEG_A + SEG_F + SEG_G + SEG_C + SEG_D,
   SEG_A + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
   SEG_A + SEG_B + SEG_C,
   SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
   SEG_A + SEG_F + SEG_G + SEG_B + SEG_C + SEG_D, // 9

   SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G, // A
   SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
   SEG_A + SEG_D + SEG_E + SEG_F,
   SEG_B + SEG_C + SEG_D + SEG_E + SEG_G,
   SEG_A + SEG_D + SEG_E + SEG_F + SEG_G,
   SEG_A + SEG_E + SEG_F + SEG_G,                 // F

   SEG_G,                                         // -
   SEG_F + SEG_E + SEG_A + SEG_B + SEG_G,         // P 
};

//constexpr uint8_t cathodes_array[DISPLAY_HC595_CATHODES] = { 3, 4, 5, 6 };
constexpr uint8_t cathodes_array[DISPLAY_HC595_CATHODES] = { 3, 5, 4, 6 };

Display_HC595 display;

void setup()
{
   Serial.begin( 115200 );
   display.begin( cathodes_array, digits_array );
}

void loop() 
{
   static uint16_t hundredths = MILLIS_TO_TICKS( 100 );
   static uint16_t seconds = MILLIS_TO_TICKS( 1000 );

   static uint16_t hundredths_counter = HUNDREDTHS_COUNTER_MOD;
   static uint16_t seconds_counter = SECONDS_COUNTER_MOD;

   static eMode mode = eMode::NONE;


   static char str_txt[] = "0123456789ABCDEF ";
   static uint8_t str_len = strlen( str_txt );
   static uint8_t str_time = 0;
   static uint8_t str_idx = 0;
   static char str_buffer[ DISPLAY_HC595_CATHODES + 1 ] = "    ";

   static uint8_t str_reps = 0;



   delay( SYSTEM_TICK );
   display.update();


   --hundredths;
   if( hundredths == 0 )
   {
      hundredths = MILLIS_TO_TICKS( 100 );

      if( mode == eMode::HUNDREDTHS and hundredths_counter > 0 )
      {
         display.clear();
         display.print_number( hundredths_counter, 2, true );

         if( hundredths_counter > 0 ) --hundredths_counter;
      }


      if( mode == eMode::TEXT and str_reps > 0 )
      {
         --str_time;
         if( str_time == 0 )
         {
            str_time = SCROLL_PERIOD;

            for( uint8_t i = 1; i < DISPLAY_HC595_CATHODES; ++i )
            {
               str_buffer[ i - 1 ] = str_buffer[ i ];
            }
            str_buffer[ DISPLAY_HC595_CATHODES - 1 ] = str_txt[ str_idx ];

            ++str_idx;
            if( str_idx == str_len )
            {
               str_idx = 0;

               --str_reps;
            }

            str_buffer[ DISPLAY_HC595_CATHODES ] = '\0';
            display.print_str( str_buffer );
         }
      }
   }

   --seconds;
   if( seconds == 0 )
   {
      seconds = MILLIS_TO_TICKS( 1000 );

      if( mode == eMode::SECONDS /*and seconds_counter > 0*/ )
      {
         display.clear();
         display.print_number( seconds_counter, 3, false );

         if( seconds_counter > 0 ) --seconds_counter;
      }
   }

   // ----------------------------------------------------------------------------

   static uint8_t state = 0;
   switch( state )
   {
      case 0:
         seconds_counter = SECONDS_COUNTER_MOD;
         mode = eMode::SECONDS;
         state = 1;
         break;

      case 1:
         if( seconds_counter == 0 )
         {
            hundredths_counter = HUNDREDTHS_COUNTER_MOD;
            mode = eMode::HUNDREDTHS;
            state = 2;
         }
         break;

      case 2:
         if( hundredths_counter == 0 )
         {
            display.clear();

            str_time = SCROLL_PERIOD;
            for( uint8_t i = 0; i < DISPLAY_HC595_CATHODES; ++i )
            {
               str_buffer[ i ] = " ";
            }
            str_idx = 0;
            str_reps = 1;
            mode = eMode::TEXT;
            state = 3;
         }
         break;

      case 3:
         if( str_reps == 0 )
         {
            state = 0;
         }
         break;
   }
}
