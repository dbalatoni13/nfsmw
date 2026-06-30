

#ifndef __USTRING_TO_NUMBER_H__
#define __USTRING_TO_NUMBER_H__

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
struct StringToNumberEntry {
    int number;
    char *string;
};

class StringToNumber {
public:
    // static void operator delete(void *mem, const char *name) {};

    StringToNumber(StringToNumberEntry *null_terminated_table);
    ~StringToNumber();

    int ConvertStringToNumber(const char *string);
    char *ConvertNumberToString(int number);


private:
    int numItems;
    StringToNumberEntry *unsorted_table;

    StringToNumberEntry *string_sorted_table;
    StringToNumberEntry *number_sorted_table;


    int BinarySearch(const char *searchFor);


    int BinarySearch(int searchFor);

public:
};

#endif
