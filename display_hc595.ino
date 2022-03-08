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
#define SECONDS_COUNTER_MOD 10

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


class Blink
{
public:
   enum class eMode: uint8_t { ONCE, REPETITIVE, FOREVER };
   enum class ePolarity: uint8_t { ACTIVE_HIGH, ACTIVE_LOW };

   Blink();
   Blink& operator=(Blink&) = delete;
   Blink(Blink&) = delete;

   void begin( uint8_t pin, ePolarity polarity = ePolarity::ACTIVE_LOW );
   void set( eMode mode, uint16_t ticks_on, uint16_t ticks_off = 0, uint16_t times = 1);
   void start();
   void stop();
   void state_machine();
   bool is_running();

   void always_on();
   void always_off();

private:
   uint8_t pin{0};

   uint16_t ticks_onMOD{0};
   uint16_t ticks_offMOD{0};
   uint16_t ticks{0};

   eMode mode{eMode::ONCE};

   ePolarity polarity{ePolarity::ACTIVE_HIGH};

   uint16_t timesMOD{0};
   uint16_t times{0};
   bool running{false};
   uint8_t state{0};

   void out( uint8_t new_state );
   // output hardware wrapper
};

Blink::Blink()
{
   // nothing
}

void Blink::begin( uint8_t pin, ePolarity polarity )
{
   this->pin = pin;
   this->polarity = polarity;

   pinMode( this->pin, OUTPUT );

   digitalWrite( this->pin, static_cast<uint8_t>(this->polarity) ^ LOW );
   // turns off the peripheral
}

void Blink::set( eMode mode, uint16_t ticks_on, uint16_t ticks_off, uint16_t times )
{
   this->mode         = mode;
   this->ticks_onMOD  = ticks_on;
   this->ticks_offMOD = ticks_off;
   this->timesMOD     = times;
}

void Blink::start()
{
   this->running = false;

   this->ticks = this->ticks_onMOD;

   if( this->mode == eMode::REPETITIVE )
   {
      this->times = this->timesMOD;
   }

   this->state = 0;
   this->running = true;

   out( HIGH );
}

void Blink::stop()
{
   this->running = false;
}

void Blink::state_machine()
{
   if( this->running )
   {
      switch( this->state )
      {
         case 0:
            --this->ticks;
            if( this->ticks == 0 )
            {
               out( LOW );

               if( this->mode == eMode::REPETITIVE or this->mode == eMode::FOREVER )
               {
                  this->ticks = this->ticks_offMOD;
                  this->state = 1;
               }
               else
               {
                  this->running = false;
               }
            }
            break;

         case 1:
            --this->ticks;
            if( this->ticks == 0 )
            {
               if( this->mode == eMode::REPETITIVE )
               {
                  --this->times;
                  if( this->times == 0 )
                  {
                     this->running = false;
                  }
               }
               else // eMode::FOREVER:
               {
                  this->state = 0;
                  this->ticks = this->ticks_onMOD;

//                  digitalWrite( this->pin, HIGH );
                  out( HIGH );
               }
            }
            break;

      } // switch state
   } // if this->running
}

bool Blink::is_running()
{
   return this->running;
}

void Blink::always_on()
{
   this->running = false;
   out( HIGH );
}

void Blink::always_off()
{
   this->running = false;
   out( LOW );
}

inline void Blink::out( uint8_t new_state )
{
   digitalWrite( this->pin, static_cast<uint8_t>(this->polarity) ^ new_state );
}



Display_HC595 display;

Blink hc595_oe;


void setup()
{
   Serial.begin( 115200 );
   display.begin( cathodes_array, digits_array );

   hc595_oe.begin( DISPLAY_HC595_OE_PIN, Blink::ePolarity::ACTIVE_LOW );
   hc595_oe.always_on();
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
   hc595_oe.state_machine();


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

      if( mode == eMode::SECONDS )
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
         state = 1;

         seconds_counter = SECONDS_COUNTER_MOD;
         mode = eMode::SECONDS;

         hc595_oe.always_on();
         break;

      case 1:
         if( seconds_counter == 0 )
         {
            state = 2;

            hundredths_counter = HUNDREDTHS_COUNTER_MOD;
            mode = eMode::HUNDREDTHS;

            hc595_oe.set( Blink::eMode::FOREVER, MILLIS_TO_TICKS( 200 ), MILLIS_TO_TICKS( 200 ) );
            hc595_oe.start();
         }
         break;

      case 2:
         if( hundredths_counter == 0 )
         {
            state = 3;

            display.clear();

            str_time = SCROLL_PERIOD;
            for( uint8_t i = 0; i < DISPLAY_HC595_CATHODES; ++i )
            {
               str_buffer[ i ] = " ";
            }
            str_idx = 0;
            str_reps = 1;
            mode = eMode::TEXT;

            hc595_oe.stop();
            hc595_oe.set( Blink::eMode::FOREVER, MILLIS_TO_TICKS( 100 ), MILLIS_TO_TICKS( 400 ) );
            hc595_oe.start();
         }
         break;

      case 3:
         if( str_reps == 0 )
         {
            state = 0;
            hc595_oe.always_on();
         }
         break;
   }
}
