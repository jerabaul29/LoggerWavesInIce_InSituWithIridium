#include "StreamAnalyzer.h"

StreamAnalyzer::StreamAnalyzer(const String string_to_compare):
    string_to_compare(string_to_compare),
    length_string_to_compare(string_to_compare.length()),
    string_index_reached(0)
    {
        PDEBVAR(string_to_compare);
    }

bool StreamAnalyzer::load_char(char new_char){
    if (new_char == string_to_compare[string_index_reached]){
        string_index_reached += 1;
        // PDEBVAR(string_index_reached)
    }
    else{
        string_index_reached = 0;
    }

    if (string_index_reached == length_string_to_compare){
            PDEBMSG("reached end of string")
            return(true);
    }
    else{
        return(false);
    }
}