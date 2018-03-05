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
