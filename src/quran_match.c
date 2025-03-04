#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <stdlib.h>

#include "quran_data.h"
#include "quran_match.h"
#include "intset.h"

static bool
quran_verse_matches(const quran_ref *ref, const quran_verse *verse)
{
    switch (ref->type) {
        case KJV_REF_SEARCH:
            return (ref->book == 0 || ref->book == verse->book) &&
                regexec(&ref->search, verse->text, 0, NULL, 0) == 0;

        case KJV_REF_EXACT:
            return ref->book == verse->book &&
                (ref->verse == 0 || ref->verse == verse->verse);

        case KJV_REF_EXACT_SET:
            return ref->book == verse->book &&
                intset_contains(ref->verse_set, verse->verse);

        case KJV_REF_RANGE:
            return ref->book == verse->book &&
                (ref->verse == 0 || verse->verse >= ref->verse) &&
                (ref->verse_end == 0 || verse->verse <= ref->verse_end);

        default:
            return false;
    }
}

#define KJV_DIRECTION_BEFORE -1
#define KJV_DIRECTION_AFTER 1

static int
quran_chapter_bounds(int i, int direction, int maximum_steps)
{
    assert(direction == KJV_DIRECTION_BEFORE || direction == KJV_DIRECTION_AFTER);

    int steps = 0;
    for ( ; 0 <= i && i < quran_verses_length; i += direction) {
        bool step_limit = (maximum_steps != -1 && steps >= maximum_steps) ||
            (direction == KJV_DIRECTION_BEFORE && i == 0) ||
            (direction == KJV_DIRECTION_AFTER && i + 1 == quran_verses_length);
        if (step_limit) {
            break;
        }

        const quran_verse *current = &quran_verses[i], *next = &quran_verses[i + direction];
        if (current->book != next->book) {
            break;
        }
        steps++;
    }
    return i;
}

static int
quran_next_match(const quran_ref *ref, int i)
{
    for ( ; i < quran_verses_length; i++) {
        const quran_verse *verse = &quran_verses[i];
        if (quran_verse_matches(ref, verse)) {
            return i;
        }
    }
    return -1;
}

static void
quran_next_addrange(quran_next_data *next, quran_range range) {
    if (next->matches[0].start == -1 && next->matches[0].end == -1) {
        next->matches[0] = range;
    } else if (range.start < next->matches[0].end) {
        next->matches[0] = range;
    } else {
        next->matches[1] = range;
    }
}

int
quran_next_verse(const quran_ref *ref, const quran_config *config, quran_next_data *next)
{
    if (next->current >= quran_verses_length) {
        return -1;
    }

    if (next->matches[0].start != -1 && next->matches[0].end != -1 && next->current >= next->matches[0].end) {
        next->matches[0] = next->matches[1];
        next->matches[1] = (quran_range){-1, -1};
    }

    if ((next->next_match == -1 || next->next_match < next->current) && next->next_match < quran_verses_length) {
        int next_match = quran_next_match(ref, next->current);
        if (next_match >= 0) {
            next->next_match = next_match;
            quran_range bounds = {
                .start = quran_chapter_bounds(next_match, KJV_DIRECTION_BEFORE, config->context_chapter ? -1 : config->context_before),
                .end = quran_chapter_bounds(next_match, KJV_DIRECTION_AFTER, config->context_chapter ? -1 : config->context_after) + 1,
            };
            quran_next_addrange(next, bounds);
        } else {
            next_match = quran_verses_length;
        }
    }

    if (next->matches[0].start == -1 && next->matches[0].end == -1) {
        return -1;
    }

    if (next->current < next->matches[0].start) {
        next->current = next->matches[0].start;
    }

    return next->current++;
}
