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
 * 2022 - fjrg76 at hotmail dot com
 */

#pragma once

#define SYSTEM_TICK (5)
#define MILLIS_TO_TICKS( x ) ( (x) / (SYSTEM_TICK) )


#define DISPLAY_HC595_CATHODES  4
#define DISPLAY_HC595_LATCH_PIN 9
#define DISPLAY_HC595_OE_PIN    10

#define SEG_A  0x80
#define SEG_B  0x01
#define SEG_C  0x10
#define SEG_D  0x04
#define SEG_E  0x08
#define SEG_F  0x40
#define SEG_G  0x20
#define SEG_DP 0x02

