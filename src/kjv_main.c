/*
kjv: Read the Word of God from your terminal

License: Public domain
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ioctl.h>

#include "kjv_config.h"
#include "kjv_data.h"
#include "kjv_ref.h"
#include "kjv_render.h"
#include "strutil.h"

const char *
usage = "usage: quran [flags] [reference...]\n"
    "\n"
    "Flags:\n"
    "  -A num  show num ayat of context after matching ayat\n"
    "  -B num  show num ayat of context before matching ayat\n"
    "  -C      show matching ayat in context of the surah\n"
    "  -e      highlighting of surah and ayah numbers\n"
    "          (default when output is a TTY)\n"
    "  -p      output to less with surah grouping, spacing, indentation,\n"
    "          and line wrapping\n"
    "          (default when output is a TTY)\n"
    "  -l      list suwar\n"
    "  -h      show help\n"
    "\n"
    "Reference:\n"
    "    <Surah>\n"
    "        Individual surah\n"
    "    <Surah>:<Ayah>\n"
    "        Individual ayah of a surah\n"
    "    <Surah>:<Ayah>[,<Ayah>]...\n"
    "        Individual ayat of a specific surah\n"
    "    <Surah>:<Ayah>-<Ayah>\n"
    "        Range of ayat in a surah\n"
    "\n"
    "    /<Search>\n"
    "        All ayat that match a pattern\n"
    "    <Surah>/<Search>\n"
    "        All ayat in a surah that match a pattern\n";

int
main(int argc, char *argv[])
{
    bool is_atty = isatty(STDOUT_FILENO) == 1;
    kjv_config config = {
        .highlighting = is_atty,
        .pretty = is_atty,

        .maximum_line_length = 80,

        .context_before = 0,
        .context_after = 0,
        .context_chapter = false,
    };

    bool list_books = false;

    opterr = 0;
    for (int opt; (opt = getopt(argc, argv, "A:B:CeplWh")) != -1; ) {
        char *endptr;
        switch (opt) {
        case 'A':
            config.context_after = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "quran: invalid flag value for -A\n\n%s", usage);
                return 1;
            }
            break;
        case 'B':
            config.context_before = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "quran: invalid flag value for -B\n\n%s", usage);
                return 1;
            }
            break;
        case 'C':
            config.context_chapter = true;
            break;
        case 'e':
            config.highlighting = true;
            break;
        case 'p':
            config.pretty = true;
            break;
        case 'l':
            list_books = true;
            break;
        case 'h':
            printf("%s", usage);
            return 0;
        case '?':
            fprintf(stderr, "quran: invalid flag -%c\n\n%s", optopt, usage);
            return 1;
        }
    }

    if (list_books) {
        for (int i = 0; i < kjv_books_length; i++) {
            kjv_book *book = &kjv_books[i];
            printf("%s (%d)\n", book->name, book->number);
        }
        return 0;
    }

    struct winsize ttysize;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ttysize) == 0 && ttysize.ws_col > 0) {
        config.maximum_line_length = ttysize.ws_col;
    }

    signal(SIGPIPE, SIG_IGN);

    if (argc == optind) {
        using_history();
        while (true) {
            char *input = readline("quran> ");
            if (input == NULL) {
                break;
            }
            add_history(input);
            kjv_ref *ref = kjv_newref();
            int success = kjv_parseref(ref, input);
            free(input);
            if (success == 0) {
                kjv_render(ref, &config);
            }
            kjv_freeref(ref);
        }
    } else {
        char *ref_str = str_join(argc-optind, &argv[optind]);
        kjv_ref *ref = kjv_newref();
        int success = kjv_parseref(ref, ref_str);
        free(ref_str);
        if (success == 0) {
            kjv_render(ref, &config);
        }
        kjv_freeref(ref);
    }

    return 0;
}
