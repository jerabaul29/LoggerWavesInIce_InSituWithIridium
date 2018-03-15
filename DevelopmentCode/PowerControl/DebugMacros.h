/*

 Copyright (c) 2015 Jean Rabault jean.rblt@gmail.com

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#include <Arduino.h>
#include "parameters.h"

#ifndef DEBUG_MACROS
#define DEBUG_MACROS

#if DEBUG
#  define PDEBMSG(x) SERIAL_DEBUG.println(); delay(5); debug_print_string(F("D ")); delay(10); debug_println_string(F(x)); delay(10);
#  define PDEBVAR(x) SERIAL_DEBUG.println(); delay(5); debug_print_string(F("D "#x" ")); delay(10); debug_println(x); delay(10);
#  define SHOW_VAR_NAME(x) SHOW_VAR_NAME_(x)
#else
#  define PDEBMSG(x) // nothing
#  define PDEBVAR(x) // nothing
#  define SHOW_VAR_NAME(x) // nothing
#endif

#define SHOW_VAR_NAME_(x) #x

// a few template designed functons, so that do not need to remember
// if needs the F() or not

void debug_println_string(const __FlashStringHelper * string_in);

void debug_print_string(const __FlashStringHelper * string_in);

template <typename T>
inline void debug_println(T data){
    SERIAL_DEBUG.println(data);
}

/*
  undeclare for strings: should use the _string versions to put in flash.
  If get a undefined reference to `void debug_println<char const*>(char const*)'
  error, this is because called this non-Fed function on a string.
*/
/*
template <>
void debug_println<const char *>(const char * data);
*/

template <typename T>
inline void debug_print(T data){
    SERIAL_DEBUG.print(data);
}

/*
  undeclare for strings: should use the _string versions to put in flash.
  If get a undefined reference to `void debug_println<char const*>(char const*)'
  error, this is because called this non-Fed function on a string.
*/
/*
template <>
void debug_print<const char *>(const char * data);
*/

void print_debug_status(void);

#endif
