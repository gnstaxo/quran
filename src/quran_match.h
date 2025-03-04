#pragma once

#include "quran_config.h"
#include "quran_ref.h"

typedef struct {
    int start;
    int end;
} quran_range;

typedef struct {
    int current;
    int next_match;
    quran_range matches[2];
} quran_next_data;

int
quran_next_verse(const quran_ref *ref, const quran_config *config, quran_next_data *next);
