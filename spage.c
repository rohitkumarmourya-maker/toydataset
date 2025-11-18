#include <string.h>
#include "spage.h"

#ifndef PF_PAGE_SIZE
#define PF_PAGE_SIZE 4096
#endif

/* Initialize a freshly-zeroed page buffer as an empty slotted page. */
int SL_InitPage(char *pagebuf)
{
    SL_PageHeader *hdr = (SL_PageHeader *)pagebuf;

    hdr->free_offset = SL_HEADER_SIZE;
    hdr->slot_count  = 0;
    hdr->free_space  = PF_PAGE_SIZE - SL_HEADER_SIZE;
    hdr->reserved    = 0;

    /* Zero the payload area (header stays valid). */
    memset(pagebuf + SL_HEADER_SIZE, 0, PF_PAGE_SIZE - SL_HEADER_SIZE);
    return 0;
}

/* Helper: return pointer to slot index i (0-based).
   Slots are stored at the high end of the page and grow downward. */
static SL_Slot *slot_ptr(char *pagebuf, int i)
{
    return (SL_Slot *)(pagebuf + PF_PAGE_SIZE - ((i + 1) * (int)sizeof(SL_Slot)));
}

/* Insert a record into the slotted page.
   On success, returns 0 and places slot id into *slotid.
   On failure (not enough room) returns -1. */
int SL_InsertRecord(char *pagebuf, const void *rec, int reclen, int *slotid)
{
    SL_PageHeader *hdr = (SL_PageHeader *)pagebuf;
    int new_slot_bytes = (hdr->slot_count + 1) * (int)sizeof(SL_Slot);
    int used_record_bytes = hdr->free_offset - SL_HEADER_SIZE; /* bytes already used for records */
    int total_used_if_insert = SL_HEADER_SIZE + used_record_bytes + new_slot_bytes + reclen;

    if (total_used_if_insert > PF_PAGE_SIZE) {
        return -1; /* insufficient space */
    }

    /* copy record into the next free slot area */
    int store_offset = hdr->free_offset;
    memcpy(pagebuf + store_offset, rec, reclen);

    /* populate slot metadata */
    SL_Slot *s = slot_ptr(pagebuf, hdr->slot_count);
    s->offset = store_offset;
    s->length = reclen;

    *slotid = hdr->slot_count;
    hdr->slot_count += 1;
    hdr->free_offset += reclen;

    /* update cached free_space */
    hdr->free_space = PF_PAGE_SIZE - (SL_HEADER_SIZE
                                     + (hdr->free_offset - SL_HEADER_SIZE)
                                     + hdr->slot_count * (int)sizeof(SL_Slot));
    return 0;
}

/* Read a record out into outbuf (caller supplies enough space).
   On success sets *outlen and returns 0. On failure returns -1. */
int SL_GetRecord(char *pagebuf, int slotid, void *outbuf, int *outlen)
{
    SL_PageHeader *hdr = (SL_PageHeader *)pagebuf;

    if (slotid < 0 || slotid >= hdr->slot_count) return -1;

    SL_Slot *s = slot_ptr(pagebuf, slotid);
    if (s->offset < 0) return -1; /* deleted/invalid slot */

    memcpy(outbuf, pagebuf + s->offset, s->length);
    *outlen = s->length;
    return 0;
}

/* Mark a slot as deleted (lazy deletion). Returns 0 on success, -1 on bad slot id. */
int SL_DeleteRecord(char *pagebuf, int slotid)
{
    SL_PageHeader *hdr = (SL_PageHeader *)pagebuf;

    if (slotid < 0 || slotid >= hdr->slot_count) return -1;

    SL_Slot *s = slot_ptr(pagebuf, slotid);
    s->offset = -1;
    s->length = 0;
    /* lazy deletion; compaction left to higher layer if needed */
    return 0;
}

/* Return number of bytes used within the page (header + slots + stored records). */
int SL_PageUtilization(char *pagebuf)
{
    SL_PageHeader *hdr = (SL_PageHeader *)pagebuf;
    int slots_bytes = hdr->slot_count * (int)sizeof(SL_Slot);
    int records_bytes = hdr->free_offset - SL_HEADER_SIZE;
    return SL_HEADER_SIZE + slots_bytes + records_bytes;
}

/* Return free space available for a new record+slot pair.
   Uses the same accounting as SL_InsertRecord. */
int SL_GetFreeSpace(char *pagebuf)
{
    SL_PageHeader *hdr = (SL_PageHeader *)pagebuf;

    int header_bytes = SL_HEADER_SIZE;
    int slots_bytes = hdr->slot_count * (int)sizeof(SL_Slot);
    int records_bytes = hdr->free_offset - SL_HEADER_SIZE;

    int used = header_bytes + slots_bytes + records_bytes;
    int free_space = PF_PAGE_SIZE - used;
    if (free_space < 0) free_space = 0;
    return free_space;
}
