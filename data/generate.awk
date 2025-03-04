BEGIN {
    #  $1 Book name
    #  $2 Book number
    #  $3 Verse number
    #  $4 Verse
    FS = "\t"

    print "/* This file is automatically generated. DO NOT EDIT. */"
    print ""
    print "#include \"quran_data.h\""
    print ""
    print "quran_verse quran_verses[] = {"

    book_count = 0
}

{
    printf("    {%d, %d, \"%s\"},\n", $2, $3, $4)
    if (!($2 in book_names)) {
        book_names[$2] = $1
        book_count++
    }
}

END {
    print "};"
    print ""
    printf("int quran_verses_length = %d;\n", NR)
    print ""

    print "quran_book quran_books[] = {"
    for (i = 1; i <= book_count; i++) {
        printf("    {%d, \"%s\"},\n", i, book_names[i])
    }
    print "};"

    print ""
    printf("int quran_books_length = %d;\n", book_count)
}
