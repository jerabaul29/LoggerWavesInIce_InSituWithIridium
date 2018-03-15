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

#include "DebugMacros.h"

// template specializations get to the .cpp, otherwise linker is confused

/*
template<>
inline void debug_println (const char * data){
    // SERIAL_DEBUG.println(F("specialized template 1"));
    // SERIAL_DEBUG.println(F(data));
    SERIAL_DEBUG.println(F("specialized template 1"));
    SERIAL_DEBUG.println(F((String) data));
}
*/

/*
  To be used on strings: should be in the flash memory.
*/
void debug_println_string(const __FlashStringHelper * string_in){
    SERIAL_DEBUG.println(string_in);
}

/*
  To be used on strings: should be in the flash memory.
*/
void debug_print_string(const __FlashStringHelper * string_in){
    SERIAL_DEBUG.print(string_in);
}
