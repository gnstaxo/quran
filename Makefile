OBJS = src/quran_main.o \
       src/quran_match.o \
       src/quran_ref.o \
       src/quran_render.o \
       src/intset.o \
       src/strutil.o \
       data/quran_data.o
CFLAGS += -Wall -Isrc/
LDLIBS += -lreadline

quran: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

src/quran_main.o: src/quran_main.c src/quran_config.h src/quran_data.h src/quran_match.h src/quran_ref.h src/quran_render.h src/strutil.h

src/quran_match.o: src/quran_match.h src/quran_match.c src/quran_config.h src/quran_data.h src/quran_ref.h

src/quran_ref.o: src/quran_ref.h src/quran_ref.c src/intset.h src/quran_data.h

src/quran_render.o: src/quran_render.h src/quran_render.c src/quran_config.h src/quran_data.h src/quran_match.h src/quran_ref.h

src/insetset.o: src/intset.h src/insetset.c

src/strutil.o: src/strutil.h src/strutil.c

data/quran_data.o: src/quran_data.h data/quran_data.c

data/quran_data.c: data/quran.tsv data/generate.awk src/quran_data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) quran
