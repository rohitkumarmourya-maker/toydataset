# README – toyDB Mini Project

## 1. Introduction

This project implements a simplified educational database storage engine called **toyDB**.  
It consists of three major components:

### **1. PF Layer (Paged File Layer)**
Responsible for:
- File I/O  
- Page allocation  
- Page buffering  
- LRU/MRU replacement  

### **2. Slotted Page Layer**
Manages records inside a page using a flexible slotted-page structure.  
(Implemented using **spage.c** and **spage.h**.)

### **3. AM Layer (Access Method Layer)**
Implements a **B+ Tree Index** supporting:
- Search  
- Insert  
- Node split  
- Range scan operations  

Each layer also has an associated experiment program that generates performance metrics in CSV format.

---

## 2. Description of Each Layer

### **2.1 PF Layer (Paged File + Buffer Manager)**

**Files:** `pf.c`, `buf.c`, `hash.c`, `pf.h`

This layer provides:
- Creation, opening and deletion of paged files  
- Reading and writing of fixed-size pages  
- Buffer management (pinning, unpinning, dirty flags)  
- Hash table for buffer lookup  
- Page replacement using LRU/MRU strategies  

**Experiment file:** `pf_experiment.c`  
**Output:** `pf_results.csv`

The PF experiment measures:
- Page read/write count  
- Buffer hit/miss ratio  
- Effects of sequential and random access  

---

### **2.2 Slotted Page Layer (Record Storage)**

**Files:** `spage.c`, `spage.h`, `analysis_tableslotted.c`

The slotted page layer manages variable-length records by maintaining:
- A slot directory  
- Offsets to each record  
- Free space tracking  
- Support for insert, delete, read  

**Experiment file:** `analysis_tableslotted.c`  
**Output:** `slotted_results.csv`

The experiment analyses:
- Space utilisation  
- Number of records that fit in one page  
- Fragmentation behaviour  

---

### **2.3 AM Layer (B+ Tree Index)**

**Files:**  
`am.c`, `amfns.c`, `amglobals.c`, `amsearch.c`,  
`amscan.c`, `amstack.c`, `aminsert.c`, `amprint.c`

The AM layer provides:
- B+ tree search  
- Insert with node split  
- Range scan operations  
- Tree printing utilities  
- Stack-based tree descent  
- Integration with PF layer for page storage  

**Experiments:**
- `bulk.c` → Bulk B+ tree build  
- `incremental.c` → Incremental B+ tree build  

**Outputs:**
- `index_bulk_results.csv`  
- `index_incremental_results.csv`  

These compare:
- Bulk-building performance vs incremental insert  
- Number of page accesses and splits  
- Index height growth  

---

## 3. How to Compile the Code

Use the following common compiler flags:

```bash
CFLAGS="-std=gnu89 \
-Wno-int-conversion -Wno-implicit-int -Wno-old-style-definition \
-Wno-builtin-declaration-mismatch -Wno-implicit-function-declaration \
-Wno-pointer-to-int-cast -Wno-return-type"

cc $CFLAGS -I./pflayer -c pflayer/buf.c  -o pflayer/buf.o
cc $CFLAGS -I./pflayer -c pflayer/hash.c -o pflayer/hash.o
cc $CFLAGS -I./pflayer -c pflayer/pf.c   -o pflayer/pf.o

ld -r -o pflayer/pflayer.o pflayer/buf.o pflayer/hash.o pflayer/pf.o

cc $CFLAGS -I./pflayer -c pflayer/pf_experiment.c -o pflayer/pf_experiment.o
cc $CFLAGS -I./pflayer pflayer/pflayer.o pflayer/pf_experiment.o -o pf_experiment

cc $CFLAGS -I./pflayer -c pflayer/spage.c                 -o pflayer/spage.o
cc $CFLAGS -I./pflayer -c pflayer/analysis_tableslotted.c -o pflayer/analysis_tableslotted.o

cc $CFLAGS -I./pflayer \
    pflayer/pflayer.o \
    pflayer/spage.o \
    pflayer/analysis_tableslotted.o \
    -o slotted_analysis

cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/am.c        -o amlayer/am.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amfns.c     -o amlayer/amfns.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amglobals.c -o amlayer/amglobals.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/aminsert.c  -o amlayer/aminsert.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amprint.c   -o amlayer/amprint.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amscan.c    -o amlayer/amscan.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amsearch.c  -o amlayer/amsearch.o
cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/amstack.c   -o amlayer/amstack.o

cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/bulk.c -o amlayer/bulk.o

cc $CFLAGS -I./pflayer -I./amlayer \
    pflayer/pflayer.o \
    amlayer/am.o amlayer/amfns.o amlayer/amglobals.o \
    amlayer/aminsert.o amlayer/amprint.o amlayer/amscan.o \
    amlayer/amsearch.o amlayer/amstack.o \
    amlayer/bulk.o \
    -o bulk

cc $CFLAGS -I./pflayer -I./amlayer -c amlayer/incremental.c -o amlayer/incremental.o

cc $CFLAGS -I./pflayer -I./amlayer \
    pflayer/pflayer.o \
    amlayer/am.o amlayer/amfns.o amlayer/amglobals.o \
    amlayer/aminsert.o amlayer/amprint.o amlayer/amscan.o \
    amlayer/amsearch.o amlayer/amstack.o \
    amlayer/incremental.o \
    -o incremental

./pf_experiment

./slotted_analysis

./bulk

./incremental

5. Output Summary

This project generates the following experiment results:

pf_results.csv — PF layer buffer performance

slotted_results.csv — Slotted page space utilisation

index_bulk_results.csv — B+ tree bulk build performance

index_incremental_results.csv — Incremental B+ tree build performance

These results can be used for performance comparison, analysis and report writing.



---

If you want, I can also:

✅ convert this into **PDF**  
✅ make it shorter / more academic  
✅ add diagrams for explanation  

Just tell me!

