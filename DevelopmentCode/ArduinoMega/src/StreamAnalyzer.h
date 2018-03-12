#include "Arduino.h"
#include "DebugMacros.h"

#ifndef STREAM_ANALYZER
#define STREAM_ANALYZER

class StreamAnalyzer{
    public:
        StreamAnalyzer(const String string_to_compare);

        bool load_char(char new_char);

    private:
        const String string_to_compare;
        const int length_string_to_compare;
        int string_index_reached;
};

#endif