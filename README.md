# toyDB-Optimization

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Type](https://img.shields.io/badge/type-DBMS%20Internal-green.svg)
![Status](https://img.shields.io/badge/status-Educational-orange.svg)

This repository contains an educational toy DBMS consisting of three storage and indexing layers. It is designed to demonstrate low-level database concepts including buffer management, page layouts, and B+ Tree indexing.

## 📂 Architecture

The system is divided into three distinct layers:

1.  **PF Layer (Paged File):** Handles file I/O, caching, and buffer management.
2.  **Slotted Page Layer:** Implements variable-length record storage using `spage.c` and `spage.h`.
3.  **AM Layer (Access Method):** Implements a B+ Tree secondary index.

### Project Layout

```text
toydb/
├── pflayer/                     # Paged File & Buffer Manager
│   ├── buf.c, buf.o
│   ├── hash.c, hash.o
│   ├── pf.c, pf.o
│   ├── pf.h, pftypes.h
│   ├── pf_experiment.c          # Buffer Manager Experiment
│   ├── spage.c, spage.h         # Slotted Page Implementation
│   ├── analysis_tableslotted.c  # Slotted Page Experiment
│   ├── test_slotted.c
│   ├── plot_pf_stats.py         # Python plotting script
│   ├── pf_results.csv           # Output
│   └── slotted_results.csv      # Output
│
├── amlayer/                     # Access Method (B+ Tree)
│   ├── am.c, am.h
│   ├── amfns.c
│   ├── amglobals.c
│   ├── aminsert.c
│   ├── amprint.c
│   ├── amsearch.c
│   ├── amscan.c
│   ├── amstack.c
│   ├── bulk.c                   # Bulk Loading Experiment
│   ├── incremental.c            # Incremental Insert Experiment
│   ├── index_bulk_results.csv   # Output
│   └── index_incremental_results.csv # Output
│
├── data/
│   └── (input data files for AM experiments)
│
└── README.md
