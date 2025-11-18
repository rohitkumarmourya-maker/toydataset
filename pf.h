/* pf.h: externs and error codes for Paged File Interface*/

#ifndef PF_H
#define PF_H

#ifndef TRUE
#define TRUE 1		
#endif
#ifndef FALSE
#define FALSE 0
#endif

/************** Error Codes *********************************/
#define PFE_OK		0	/* OK */
#define PFE_NOMEM	-1	/* no memory */
#define PFE_NOBUF	-2	/* no buffer space */
#define PFE_PAGEFIXED 	-3	/* page already fixed in buffer */
#define PFE_PAGENOTINBUF -4	/* page to be unfixed is not in the buffer */
#define PFE_UNIX	-5	/* unix error */
#define PFE_INCOMPLETEREAD -6	/* incomplete read of page from file */
#define PFE_INCOMPLETEWRITE -7	/* incomplete write of page to file */
#define PFE_HDRREAD	-8	/* incomplete read of header from file */
#define PFE_HDRWRITE	-9	/* incomplte write of header to file */
#define PFE_INVALIDPAGE -10	/* invalid page number */
#define PFE_FILEOPEN	-11	/* file already open */
#define	PFE_FTABFULL	-12	/* file table is full */
#define PFE_FD		-13	/* invalid file descriptor */
#define PFE_EOF		-14	/* end of file */
#define PFE_PAGEFREE	-15	/* page already free */
#define PFE_PAGEUNFIXED	-16	/* page already unfixed */

/* Internal error: please report to the TA */
#define PFE_PAGEINBUF	-17	/* new page to be allocated already in buffer */
#define PFE_HASHNOTFOUND -18	/* hash table entry not found */
#define PFE_HASHPAGEEXIST -19	/* page already exist in hash table */


///* page size */
//#define PF_PAGE_SIZE	4096

/* Replacement policies (must match pftypes.h) */
#define REPL_LRU 1
#define REPL_MRU 2

/************ PF Statistics Struct ************/
typedef struct {
    long logical_reads;
    long logical_writes;
    long physical_reads;
    long physical_writes;
    long cache_hits;
    long cache_misses;
} PFStats;

/******************* Extern Globals *******************/
extern int PFerrno;
extern PFStats PF_stats;
extern int PF_BUFFER_SIZE;        /* dynamic buffer pool size */
extern int PF_Replacement_Policy; /* LRU (default) / MRU */

/******************* PF Layer API ********************/
extern void PF_Init();
extern void PF_PrintError(char *);

/* Assignment-required additions */
void PF_SetReplacementPolicy(int policy);
void PF_SetBufferSize(int size);

/* mark dirty explicitly */
int PF_MarkDirty(int fd, int pagenum);

/* stats */
void PF_PrintStats(void);
void PF_ResetStats(void);

int PF_CreateFile(char *fname);
int PF_DestroyFile(char *fname);
int PF_OpenFile(char *fname);
int PF_CloseFile(int fd);
int PF_AllocPage(int fd, int *pagenum, char **pagebuf);
int PF_GetThisPage(int fd, int pagenum, char **pagebuf);
int PF_UnfixPage(int fd, int pagenum, int dirty);

#endif
