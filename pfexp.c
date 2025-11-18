#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pf.h"
#include "pftypes.h"

#define FNAME "expfile"

static void
execute_run(int policy, int writeRatio, int totalPages,
            int ops, int bufLimit, FILE *out)
{
    int fd, pg, i;
    char *ptr;

    PF_SetBufferSize(bufLimit);
    PF_SetReplacementPolicy(policy);
    PF_ResetStats();

    if (PF_CreateFile(FNAME) != PFE_OK) {
        PF_PrintError("Could not create file");
        exit(1);
    }

    fd = PF_OpenFile(FNAME);
    if (fd < 0) {
        PF_PrintError("open");
        exit(1);
    }

    /* Pre-allocate pages */
    for (i = 0; i < totalPages; ++i) {
        if (PF_AllocPage(fd, &pg, &ptr) != PFE_OK) {
            PF_PrintError("alloc");
            exit(1);
        }
        ptr[0] = 0;
        PF_UnfixPage(fd, pg, TRUE);
    }

    /* Random accesses */
    for (i = 0; i < ops; ++i) {
        int target = rand() % totalPages;

        if (PF_GetThisPage(fd, target, &ptr) != PFE_OK) {
            PF_PrintError("get");
            exit(1);
        }

        int doWrite = (rand() % 100) < writeRatio;
        if (doWrite) {
            ptr[0]++;
        }

        PF_UnfixPage(fd, target, doWrite);
    }

    /* Dump one CSV line */
    fprintf(out,
        "%d,%d,%d,%d,"
        "%ld,%ld,%ld,%ld,%ld,%ld\n",
        policy,
        bufLimit,
        totalPages,
        writeRatio,
        PF_stats.logical_reads,
        PF_stats.logical_writes,
        PF_stats.physical_reads,
        PF_stats.physical_writes,
        PF_stats.cache_hits,
        PF_stats.cache_misses
    );

    PF_CloseFile(fd);
    PF_DestroyFile(FNAME);
}

int main(void)
{
    FILE *csv = fopen("pf_results.csv", "w");
    if (!csv) {
        perror("pf_results.csv");
        return 1;
    }

    fprintf(csv,
        "policy,bufferSize,nPages,writePct,"
        "logicalReads,logicalWrites,"
        "physicalReads,physicalWrites,"
        "cacheHits,cacheMisses\n");

    srand((unsigned)time(NULL));

    int policies[] = {REPL_LRU, REPL_MRU};
    int mixes[]    = {0, 10, 30, 50, 70, 90};

    const int bsize = 20;
    const int pages = 200;
    const int ops   = 20000;

    for (int pi = 0; pi < 2; ++pi) {
        for (int mi = 0; mi < 6; ++mi) {
            execute_run(policies[pi], mixes[mi], pages, ops, bsize, csv);
            fflush(csv);
        }
    }

    fclose(csv);
    printf("Done. Output written to pf_results.csv\n");
    return 0;
}
