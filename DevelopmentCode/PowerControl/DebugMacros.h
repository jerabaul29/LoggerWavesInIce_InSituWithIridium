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
