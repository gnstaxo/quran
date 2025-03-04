#pragma once

typedef struct {
    int number;
    char *name;
} quran_book;

typedef struct {
    int book;
    int verse;
    char *text;
} quran_verse;

extern quran_verse quran_verses[];

extern int quran_verses_length;

extern quran_book quran_books[];

extern int quran_books_length;
