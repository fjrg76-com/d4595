
#include "Display_HC595.hpp"

Display_HC595::Display_HC595()
{

}

void Display_HC595::begin( const uint8_t* cathodes, const uint8_t* digits )
{
   this->cathodes = cathodes;
   this->digits = digits;

   pinMode( DISPLAY_HC595_LATCH_PIN, OUTPUT );
   digitalWrite( DISPLAY_HC595_LATCH_PIN, LOW );

   SPI.begin();

   for( uint8_t i = 0; i < DISPLAY_HC595_CATHODES; ++i )
   {
      pinMode( this->cathodes[ i ], OUTPUT );
      digitalWrite( this->cathodes[ i ], LOW );
   }

   pinMode( DISPLAY_HC595_OE_PIN, OUTPUT );
   digitalWrite( DISPLAY_HC595_OE_PIN, LOW );
}

void Display_HC595::update()
{
   static uint8_t current_digit = 0;

//   digitalWrite( DISPLAY_HC595_OE_PIN, HIGH );

   for( uint8_t i = 0; i < DISPLAY_HC595_CATHODES; ++i )
   {
      digitalWrite( this->cathodes[ i ], LOW );
   }

   digitalWrite( DISPLAY_HC595_LATCH_PIN, LOW );
    SPI.beginTransaction( SPISettings( 16000000, MSBFIRST, SPI_MODE0 ) );
    SPI.transfer( this->memory[ current_digit ] );
    SPI.endTransaction();
   digitalWrite( DISPLAY_HC595_LATCH_PIN, HIGH );

   digitalWrite( this->cathodes[ current_digit ], HIGH );

//   digitalWrite( DISPLAY_HC595_OE_PIN, LOW );

   ++current_digit;
   if( current_digit == DISPLAY_HC595_CATHODES ) current_digit = 0;

}

void Display_HC595::print_str( uint8_t* str )
{
   uint8_t cont = 0;
   // moves through this->memory[]

   uint8_t idx = 0;
   // moves through str[]

   char car = str[ idx ];

   for( uint8_t i = DISPLAY_HC595_CATHODES; i > 0; --i )
   {
      this->memory[ i ] = 0;
   }

   bool prev_was_char = false;

   while( car && cont < DISPLAY_HC595_CATHODES )
   {
      if( 0x30 <= car && car < 0x3A )
      {
         this->memory[ cont ] = this->digits[ car-0x30 ];
         prev_was_char = true;
      }
      else if( 0x41 <= car && car < 0x47 )
      {
         this->memory[ cont ] = this->digits[ (car-0x41+10) ];
         prev_was_char = true;
      }
      else if( 0x61 <= car && car < 0x67 )
      {
         this->memory[ cont ] = this->digits[ (car-0x61+10) ];
         prev_was_char = true;
      }
      else if( car == '.' )
      {
         if( not prev_was_char )
         {
            this->memory[ cont ] = SEG_DP;
         }
         else
         {
            --cont;
            this->memory[ cont ] += SEG_DP;

            prev_was_char = false;
         }
      }
      else if( car == '-' )
      {
         this->memory[ cont ] = SEG_G;
         prev_was_char = true;
      }
      else
      {
         this->memory[ cont ] = 0;
         prev_was_char = false;
      }


      ++cont;

      ++idx;
      car = str[ idx ];

      if( cont == DISPLAY_HC595_CATHODES && car == '.' )
      {
         this->memory[ cont - 1 ] += SEG_DP;
      }
   }
}

void Display_HC595::print_number( uint16_t num, uint8_t dp_pos, bool leading_zero )
{
   for( uint8_t i = DISPLAY_HC595_CATHODES; i > 0; --i )
   {
      uint16_t div = num / 10;
      uint16_t digit = num - div * 10;
      num = div;

      if( leading_zero == false and digit == 0 and num == 0 )
      {
         // prints out the most right zero (units)
         if( i == DISPLAY_HC595_CATHODES )
         {
            this->memory[ i - 1 ] = this->digits[0] + ((dp_pos==(i-1)) ? SEG_DP : 0);
         }

         // doesn't print out any other zero than that in the units, and prints out the decimal point.
         // The decimal point can be located anywhere, not only in the right most position
         else
         {
            this->memory[ i - 1 ] = ((dp_pos==(i-1)) ? SEG_DP : 0);

            if( this->memory[ i - 1 ] > 0 ) return;
            // the decimal point has been printed out, nothing else to do
         }
      }

      else
      {
         this->memory[ i - 1 ] = this->digits[ digit ] + ((dp_pos==(i-1)) ? SEG_DP : 0);
      }
   }
}


void Display_HC595::clear()
{
   for( uint8_t i = DISPLAY_HC595_CATHODES; i > 0; --i )
   {
      this->memory[ i - 1 ] = 0;
   }
}
