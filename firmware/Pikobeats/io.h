
// Copyright 2023 Rich Heslip
//
// Author: Rich Heslip 
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// I/O pin definitions for Pikocore - I'm using VCC-GND YD-RP2040 board from AliExpress


#ifndef IO_H_
#define IO_H_

//#define CPU_USE 15
#define PWMOUT 22

// analog inputs for voltage control - range approx 0-5v
// 
#define AIN0 	26
#define AIN1 	27
//#define AIN2 	28
//#define AIN3 	29 // not available on standard Pico board



// button inputs
#define BUTTON0  0 // key1 input on schematic
#define BUTTON1	2
#define BUTTON2	4
#define BUTTON3	6
#define BUTTON4 8
#define BUTTON5	10
#define BUTTON6	12
#define BUTTON7 14  // for the Pico board
#define SHIFTBUTTON 28 // was 24 USR button on VCC-GND boards

#define LED0 1 // LED1 output on schematic
#define LED1 3
#define LED2 5
#define LED3 7
#define LED4 9
#define LED5 11
#define LED6 13
#define LED7 15


#define CLOCKOUT 16

// MIDI serial port pins - not really MIDI but the serial port is exposed on the first two jacks
#define MIDIRX 17
#define MIDITX 16

#endif // IO_H_
