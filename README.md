# README – toyDB Mini Project

## 1. Introduction

This project implements a simplified educational database storage engine called toyDB.

It consists of three major components:

1. PF Layer (Paged File Layer) Responsible for:
    File I/O
    Page allocation
    Page buffering
    LRU/MRU replacement
2. Slotted Page Layer Manages records inside a page using a flexible slotted-page structure.
    (Implemented using spage.c and spage.h.)
3. AM Layer (Access Method Layer) Implements a B+ Tree Index supporting:
    Search
    Insert
    Node splitting
    Range scan operations
Each layer also has an associated experiment program that generates performance metrics in CSV
format.

## 2. Description of Each Layer

2.1 PF Layer (Paged File + Buffer Manager)
Files: pf.c, buf.c, hash.c, pf.h

This layer provides:
Creation, opening and deletion of paged files
Reading and writing of fixed-size pages
Buffer management (pinning, unpinning, dirty flags)
Hash table for buffer lookup
Page replacement using LRU/MRU policies
Experiment file: pf_experiment.c Output: pf_results.csv

The PF experiment measures:
Page read/write count
Buffer hit/miss ratio
Sequential vs random access cost


2.2 Slotted Page Layer (Record Storage)
Files: spage.c, spage.h, analysis_tableslotted.c

This layer manages variable-length records inside a page using:
Slot directory
Offsets for each record
Free space tracking
Insert/Delete/Read operations
Experiment file: analysis_tableslotted.c Output: slotted_results.csv

The experiment evaluates:
Space utilisation
Number of records per page for different record sizes
Fragmentation behaviour

2.3 AM Layer (B+ Tree Index)
Files: am.c, amfns.c, amglobals.c, amsearch.c, amscan.c, amstack.c, aminsert.c,
amprint.c

This layer provides:
B+ Tree insertion
Node splitting
Search operations
Range scans
Tree printing utilities
Integration with PF layer
Experiments:
bulk.c → Bulk B+ tree build
incremental.c → Incremental build
Outputs:
index_bulk_results.csv
index_incremental_results.csv

## 3. How to Compile the Code

Use the following compiler flags for all files. Run this command in your terminal first to set the
variable:

```
export CFLAGS="-std=gnu89 \
-Wno-int-conversion -Wno-implicit-int -Wno-old-style-definition \
-Wno-builtin-declaration-mismatch -Wno-implicit-function-declaration \
-Wno-pointer-to-int-cast -Wno-return-type"
```
3.1 Compile PF Layer

```
cc $CFLAGS -I./pflayer -c pflayer/buf.c -o pflayer/buf.o
cc $CFLAGS -I./pflayer -c pflayer/hash.c -o pflayer/hash.o
cc $CFLAGS -I./pflayer -c pflayer/pf.c -o pflayer/pf.o
ld -r -o pflayer/pflayer.o pflayer/buf.o pflayer/hash.o pflayer/pf.o
```
3.2 Compile PF Experiment

```
cc $CFLAGS -I./pflayer -c pflayer/pf_experiment.c -o pflayer/pf_experiment.o
cc $CFLAGS -I./pflayer pflayer/pflayer.o pflayer/pf_experiment.o -o pf_experiment
```
3.3 Compile Slotted Page Layer

```
cc $CFLAGS -I./pflayer -c pflayer/spage.c                 -o pflayer/spage.o
cc $CFLAGS -I./pflayer -c pflayer/analysis_tableslotted.c -o pflayer/analysis_tableslotted.o

cc $CFLAGS -I./pflayer \
    pflayer/pflayer.o \
    pflayer/spage.o \
    pflayer/analysis_tableslotted.o \
    -o slotted_analysis

```
3.4 Compile AM Layer

```
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/am.c -o amlayer/am.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amfns.c -o amlayer/amfns.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amglobals.c -o amlayer/amglobals.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/aminsert.c -o amlayer/aminsert.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amprint.c -o amlayer/amprint.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amscan.c -o amlayer/amscan.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amsearch.c -o amlayer/amsearch.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amstack.c -o amlayer/amstack.o
```
3.5 Compile AM Experiments
Bulk Build

```
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/bulk.c -o amlayer/bulk.o

cc $CFLAGS -I./pflayer -I./amlayer \
    pflayer/pflayer.o \
    amlayer/am.o amlayer/amfns.o amlayer/amglobals.o \
    amlayer/aminsert.o amlayer/amprint.o amlayer/amscan.o \
    amlayer/amsearch.o amlayer/amstack.o \
    amlayer/bulk.o \
    -o bulk

```
Incremental Build

```
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/incremental.c -o amlayer/incremental.o
cc $CFLAGS -I./pflayer -I./amlayer \
pflayer/pflayer.o \
amlayer/am.o amlayer/amfns.o amlayer/amglobals.o \
amlayer/aminsert.o amlayer/amprint.o amlayer/amscan.o \
amlayer/amsearch.o amlayer/amstack.o \
amlayer/incremental.o \
-o incremental
```
## 4. How to Run the Code

4.1 Run PF Experiment

```
./pf_experiment
```
Produces: pf_results.csv

4.2 Run Slotted Page Experiment

```
./slotted_analysis
```
Produces: slotted_results.csv

4.3 Run Bulk Index Build

```
./bulk
```
Produces: index_bulk_results.csv

4.4 Run Incremental Index Build

```
./incremental
```
Produces: index_incremental_results.csv

## RUNNING ALL EXPERIMENTS
```
./pf_experiment      # pf_results.csv
./slotted_analysis   # slotted_results.csv
./bulk               # index_bulk_results.csv
./incremental        # index_incremental_results.csv

```

## 5. Output Summary


Generated experiment outputs:
pf_results.csv — PF layer buffer performance
slotted_results.csv — Slotted page space utilisation
index_bulk_results.csv — B+ Tree bulk build performance
index_incremental_results.csv — Incremental B+ Tree build performance


