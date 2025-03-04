#pragma once

#include <regex.h>

#include "intset.h"

#define KJV_REF_SEARCH 1
#define KJV_REF_EXACT 2
#define KJV_REF_EXACT_SET 3
#define KJV_REF_RANGE 4

typedef struct quran_ref {
    int type;
    unsigned int book;
    unsigned int verse;
    unsigned int verse_end;
    intset *verse_set;
    char *search_str;
    regex_t search;
} quran_ref;

quran_ref *
quran_newref();

void
quran_freeref(quran_ref *ref);

int
quran_parseref(quran_ref *ref, const char *ref_str);
