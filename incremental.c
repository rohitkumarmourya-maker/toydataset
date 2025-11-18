#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../pflayer/pf.h"
#include "am.h"

#define DATAFILE   "idatafile"
#define INDEXFILE  "idatafile.1"

int main(void)
{
    /* Seed PRNG for key generation */
    srand((unsigned)time(NULL));

    PF_Init();
    PF_ResetStats();

    /* Build the index file */
    if (AM_CreateIndex(DATAFILE, 1, 'i', sizeof(int)) != AME_OK) {
        AM_PrintError("CreateIndex");
        return 1;
    }

    /* Open the index through PF layer */
    int idxFd = PF_OpenFile(INDEXFILE);
    if (idxFd < 0) {
        PF_PrintError("PF_OpenFile");
        return 1;
    }

    /* CSV logging setup */
    FILE *csv = fopen("index_incremental_results.csv", "w");
    if (!csv) {
        perror("index_incremental_results.csv");
        PF_CloseFile(idxFd);
        return 1;
    }

    fprintf(csv,
            "recordsInserted,logicalReads,logicalWrites,"
            "physicalReads,physicalWrites,cacheHits,cacheMisses\n");

    int recId = 1;

    /* Insert entries one by one */
    for (int n = 1; n <= 10000; ++n) {

        int key = rand() % 100000;

        if (AM_InsertEntry(idxFd, 'i', sizeof(int),
                           (char *)&key, recId) != AME_OK) {
            AM_PrintError("InsertEntry");
            fclose(csv);
            PF_CloseFile(idxFd);
            return 1;
        }

        recId++;

        /* Emit stats snapshot every 500 inserts */
        if (n % 500 == 0) {
            fprintf(csv, "%d,%ld,%ld,%ld,%ld,%ld,%ld\n",
                    n,
                    PF_stats.logical_reads,
                    PF_stats.logical_writes,
                    PF_stats.physical_reads,
                    PF_stats.physical_writes,
                    PF_stats.cache_hits,
                    PF_stats.cache_misses);
        }
    }

    fclose(csv);
    PF_CloseFile(idxFd);

    printf("Incremental index build finished.\n");
    printf("Stats written to index_incremental_results.csv\n");
    return 0;
}
