#include <ctype.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quran_data.h"
#include "quran_ref.h"

quran_ref *
quran_newref()
{
    return calloc(1, sizeof(quran_ref));
}

void
quran_freeref(quran_ref *ref)
{
    if (ref) {
        free(ref->search_str);
        regfree(&ref->search);
        free(ref);
    }
}

int
quran_parseref(quran_ref *ref, const char *ref_str)
{
    // 1. <book>
    // 2. <book>:?<chapter>
    // 3. <book>:?<chapter>:<verse>
    // 3a. <book>:?<chapter>:<verse>[,<verse>]...
    // 4. <book>:?<chapter>-<chapter>
    // 5. <book>:?<chapter>:<verse>-<verse>
    // 6. <book>:?<chapter>:<verse>-<chapter>:<verse>
    // 7. /<search>
    // 8. <book>/search
    // 9. <book>:?<chapter>/search

    ref->type = 0;
    ref->book = 0;
    ref->verse = 0;
    ref->verse_end = 0;
    intset_free(ref->verse_set);
    ref->verse_set = NULL;
    free(ref->search_str);
    ref->search_str = NULL;
    regfree(&ref->search);

    int n = 0;
    if (sscanf(ref_str, "%u %n", &ref->book, &n) == 1) {
        // 1, 2, 3, 3a, 4, 5, 6, 8, 9
        ref_str = &ref_str[n];
    } else if (ref_str[0] == '/') {
        // 7
        goto search;
    } else {
        return 1;
    }

    if (sscanf(ref_str, ": %u %n", &ref->verse, &n) == 1) {
        // 3, 3a, 5, 6
        ref_str = &ref_str[n];
    } else if (ref_str[0] == '/') {
        // 9
        goto search;
    } else if (ref_str[0] == '\0') {
        // 2
        ref->type = KJV_REF_EXACT;
        return 0;
    } else {
        return 1;
    }

    unsigned int value;
    int ret = sscanf(ref_str, "- %u %n", &value, &n);
    if (ret == 1 && ref_str[n] == '\0') {
        // 5
        ref->verse_end = value;
        ref->type = KJV_REF_RANGE;
        return 0;
    } else if (ref_str[0] == '\0') {
        // 3
        ref->type = KJV_REF_EXACT;
        return 0;
    } else if (sscanf(ref_str, ", %u %n", &value, &n) == 1) {
        // 3a
        ref->verse_set = intset_new();
        intset_add(ref->verse_set, ref->verse);
        intset_add(ref->verse_set, value);
        ref_str = &ref_str[n];
        while (true) {
            if (sscanf(ref_str, ", %u %n", &value, &n) != 1) {
                break;
            }
            intset_add(ref->verse_set, value);
            ref_str = &ref_str[n];
        }
        if (ref_str[0] != '\0') {
            return 1;
        }
        ref->type = KJV_REF_EXACT_SET;
        return 0;
    } else {
        return 1;
    }

search:
    ref->type = KJV_REF_SEARCH;
    if (regcomp(&ref->search, &ref_str[1], REG_EXTENDED|REG_ICASE|REG_NOSUB) != 0) {
        return 2;
    }
    ref->search_str = strdup(&ref_str[1]);
    return 0;
}
