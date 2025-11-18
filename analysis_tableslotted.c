#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spage.h"
#include "pftypes.h"

#define PAGE_SIZE PF_PAGE_SIZE

int main(void)
{
    FILE *csv = fopen("slotted_results.csv", "w");
    if (!csv) {
        perror("slotted_results.csv");
        return 1;
    }

    /* CSV output header */
    fprintf(csv, "record_size,records_inserted,free_space\n");

    /* Test various record lengths */
    int sizes[] = {20, 40, 80, 160, 200, 300, 500, 800};
    int n = sizeof(sizes) / sizeof(int);

    char page[PAGE_SIZE];

    for (int idx = 0; idx < n; ++idx) {

        /* prepare a clean page */
        SL_InitPage(page);

        int rsize = sizes[idx];
        int inserted = 0;
        int slotid = -1;

        /* create a record buffer of given size */
        char *payload = malloc(rsize);
        memset(payload, 'A', rsize);

        /* insert repeatedly until the page fills up */
        while (SL_InsertRecord(page, payload, rsize, &slotid) == 0) {
            inserted++;
        }

        /* compute effective leftover bytes */
        int leftover = SL_GetFreeSpace(page);

        /* write a row in CSV */
        fprintf(csv, "%d,%d,%d\n", rsize, inserted, leftover);

        free(payload);
    }

    fclose(csv);
    printf("Slotted-page experiment complete → slotted_results.csv\n");
    return 0;
}
