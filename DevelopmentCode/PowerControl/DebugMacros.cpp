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
