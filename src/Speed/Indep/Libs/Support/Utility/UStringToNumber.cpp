#include <stdlib.h>
#include <string.h>

#include "Speed/Indep/Libs/Support/Utility/UStringToNumber.h"

static int compare_entry_string(const void *e1, const void *e2) {
    return strcmp(((const StringToNumberEntry *)e1)->string, ((const StringToNumberEntry *)e2)->string);
}

static int compare_entry_number(const void *e1, const void *e2) {
    return ((const StringToNumberEntry *)e1)->number - ((const StringToNumberEntry *)e2)->number;
}

StringToNumber::StringToNumber(StringToNumberEntry *null_terminated_table) {
    int count = 0;

    for (StringToNumberEntry *entry = null_terminated_table; entry->string != NULL; entry++) {
        count++;
    }

    numItems = count;
    unsorted_table = null_terminated_table;

    if (count > 30) {
        string_sorted_table = (StringToNumberEntry *)gFastMem.Alloc(count * 16, "StringToNumber");
        number_sorted_table = string_sorted_table + count;

        for (int i = 0; i < numItems; i++) {
            string_sorted_table[i] = null_terminated_table[i];
            number_sorted_table[i] = null_terminated_table[i];
        }

        qsort(string_sorted_table, numItems, sizeof(StringToNumberEntry), compare_entry_string);
        qsort(number_sorted_table, numItems, sizeof(StringToNumberEntry), compare_entry_number);
    } else {
        string_sorted_table = NULL;
        number_sorted_table = NULL;
    }
}

char *StringToNumber::ConvertNumberToString(int number) {
    if (number_sorted_table != NULL) {
        int index = BinarySearch(number);

        if (index != -1) {
            return number_sorted_table[index].string;
        }
    } else {
        for (int i = 0; i < numItems; i++) {
            if (unsorted_table[i].number == number) {
                return unsorted_table[i].string;
            }
        }
    }

    return NULL;
}

int StringToNumber::BinarySearch(int searchFor) {
    int low = 0;
    int high = numItems - 1;
    int result = -1;

    while (high >= low) {
        result = (low + high) >> 1;

        if (searchFor > number_sorted_table[result].number) {
            low = result + 1;
        } else if (searchFor < number_sorted_table[result].number) {
            high = result - 1;
        } else {
            return result;
        }
    }

    if (searchFor == number_sorted_table[result].number) {
        return result;
    }

    return -1;
}
