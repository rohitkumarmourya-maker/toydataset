#ifndef SLOTTED_H
#define SLOTTED_H

#include <stdint.h>
#include "pf.h"

/*
 * The page begins with a compact header (8 bytes total).
 * After the header, records grow upward from SL_HEADER_SIZE,
 * while slot entries are stored at the end of the page and grow downward.
 */
#define SL_HEADER_SIZE 8   /* update if header layout is modified */

/* Per-page metadata stored at the front of the slotted page. */
typedef struct {
    int16_t free_offset;   /* where the next record payload will be written */
    int16_t slot_count;    /* total number of slots (deleted ones included) */
    int16_t free_space;    /* cached available space inside the page */
    int16_t reserved;      /* kept for alignment/future use */
} SL_PageHeader;

/* Slot directory entry, stored at the high end of the page. */
typedef struct {
    int16_t offset;        /* byte offset of record; -1 indicates a deleted slot */
    int16_t length;        /* record length in bytes */
} SL_Slot;

/* ---- Slotted-page API ---- */
int SL_InitPage(char *pagebuf);                        /* initialize empty slotted page           */
int SL_InsertRecord(char *pagebuf, const void *rec,
                    int reclen, int *slotid);          /* append a record and assign slot id      */
int SL_GetRecord(char *pagebuf, int slotid,
                 void *outbuf, int *outlen);           /* fetch record from slot                  */
int SL_DeleteRecord(char *pagebuf, int slotid);        /* mark slot as deleted (lazy removal)     */
int SL_PageUtilization(char *pagebuf);                 /* return bytes currently used on the page */

#endif
