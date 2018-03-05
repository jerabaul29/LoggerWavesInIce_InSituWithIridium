#include <Arduino.h>
#include "parameters.h"

#ifndef DEBUG_MACROS
#define DEBUG_MACROS

#if DEBUG
#  define PLDDEB(x) debug_println("D "); debug_println(x);
#  define PDDEB(x) SERIAL_DEBUG.print(F("D ")); SERIAL_DEBUG.print(x);
#  define PLDEB(x) debug_println(x);
#  define PDEB(x) SERIAL_DEBUG.print(x);
#else
#  define PLDDEB(x) // nothing
#  define PDDEB(x) // nothing
#  define PLDEB(x) // nothing
#  define PDEB(x) // nothing
#endif

PROGMEM

// a few template designed functions, so that do not need to remember
// if needs the F() or not

template <typename T>
inline void debug_println(T data){
    SERIAL_DEBUG.println("general template");
    SERIAL_DEBUG.println(data);
}

template <typename T>
inline void debug_print(T data){
    SERIAL_DEBUG.println("general template 2");
    SERIAL_DEBUG.print(data);
}

#endif
