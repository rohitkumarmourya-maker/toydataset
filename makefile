# ===============================
#  AM Layer Build Makefile
# ===============================

CFLAGS = -std=gnu89 -Wno-implicit-int -Wno-old-style-definition \
         -Wno-int-conversion -Wno-implicit-function-declaration \
         -I. -I../pflayer

# -------------------------------
# Build all AM modules
# -------------------------------
AM_OBJS = am.o amfns.o amsearch.o aminsert.o amstack.o \
          amglobals.o amscan.o amprint.o

# Produce monolithic amlayer.o (used by bulk/incremental builders)
amlayer.o : $(AM_OBJS)
	ld -r $(AM_OBJS) -o amlayer.o


# -------------------------------
# Individual source compilations
# -------------------------------

am.o : am.c am.h ../pflayer/pf.h
	cc $(CFLAGS) -c am.c

amfns.o : amfns.c am.h ../pflayer/pf.h
	cc $(CFLAGS) -c amfns.c

amsearch.o : amsearch.c am.h ../pflayer/pf.h
	cc $(CFLAGS) -c amsearch.c

aminsert.o : aminsert.c am.h ../pflayer/pf.h
	cc $(CFLAGS) -c aminsert.c

amscan.o : amscan.c am.h ../pflayer/pf.h
	cc $(CFLAGS) -c amscan.c

amstack.o : amstack.c am.h
	cc $(CFLAGS) -c amstack.c

amglobals.o : amglobals.c am.h
	cc $(CFLAGS) -c amglobals.c

amprint.o : amprint.c am.h ../pflayer/pf.h
	cc $(CFLAGS) -c amprint.c


# -------------------------------
# AM test program (optional)
# -------------------------------
a.out : $(AM_OBJS) ../pflayer/pflayer.o main.o
	cc $(CFLAGS) $(AM_OBJS) ../pflayer/pflayer.o main.o -o a.out

main.o : main.c am.h ../pflayer/pf.h
	cc $(CFLAGS) -c main.c


# -------------------------------
# Build Index Programs
# -------------------------------

build_index_bulk : amlayer.o ../pflayer/pflayer.o build_index_bulk.c
	cc $(CFLAGS) amlayer.o ../pflayer/pflayer.o build_index_bulk.c -o build_index_bulk

build_index_incremental : amlayer.o ../pflayer/pflayer.o build_index_incremental.c
	cc $(CFLAGS) amlayer.o ../pflayer/pflayer.o build_index_incremental.c -o build_index_incremental


# -------------------------------
# Clean
# -------------------------------
clean:
	rm -f *.o a.out amlayer.o build_index_bulk build_index_incremental
