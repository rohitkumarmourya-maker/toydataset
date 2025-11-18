/* buf.c: buffer management routines. The interface routines are:
PFbufGet(), PFbufUnfix(), PFbufAlloc(), PFbufReleaseFile(), PFbufUsed() and
PFbufPrint() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pf.h"
#include "pftypes.h"
extern int PF_BUFFER_SIZE;
extern int PF_Replacement_Policy;
extern PFStats PF_stats;


static int PFnumbpage = 0;	/* # of buffer pages in memory */
static PFbpage *PFfirstbpage= NULL;	/* ptr to first buffer page, or NULL */
static PFbpage *PFlastbpage = NULL;	/* ptr to last buffer page, or NULL */
static PFbpage *PFfreebpage= NULL;	/* list of free buffer pages */

//extern char *malloc();

static void PFbufInsertFree(bpage)
PFbpage *bpage;
/****************************************************************************
SPECIFICATIONS:
	Insert the buffer page pointed by "bpage" into the free list.

AUTHOR: clc
*****************************************************************************/
{
	bpage->nextpage = PFfreebpage;
	PFfreebpage = bpage;
}


static void PFbufLinkHead(PFbpage *bpage)
{
    bpage->nextpage = PFfirstbpage;
    bpage->prevpage = NULL;
    if (PFfirstbpage != NULL)
        PFfirstbpage->prevpage = bpage;
    PFfirstbpage = bpage;
    if (PFlastbpage == NULL)
        PFlastbpage = bpage;
}

void PFbufUnlink(PFbpage *bpage)
{
    if (PFfirstbpage == bpage)
        PFfirstbpage = bpage->nextpage;

    if (PFlastbpage == bpage)
        PFlastbpage = bpage->prevpage;

    if (bpage->nextpage != NULL)
        bpage->nextpage->prevpage = bpage->prevpage;

    if (bpage->prevpage != NULL)
        bpage->prevpage->nextpage = bpage->nextpage;

    bpage->prevpage = bpage->nextpage = NULL;
}

/* choose victim based on LRU/MRU */
static PFbpage *choose_victim()
{
    PFbpage *tb;
    if (PF_Replacement_Policy == REPL_LRU) {
        for (tb = PFlastbpage; tb != NULL; tb = tb->prevpage)
            if (!tb->fixed) return tb;
    } else {
        for (tb = PFfirstbpage; tb != NULL; tb = tb->nextpage)
            if (!tb->fixed) return tb;
    }
    return NULL;
}



static int PFbufInternalAlloc(PFbpage **bpage, int (*writefcn)())
{
    PFbpage *tbpage;
    int error;

    if (PFfreebpage != NULL){
        *bpage = PFfreebpage;
        PFfreebpage = (*bpage)->nextpage;
    }
    else if (PFnumbpage < PF_BUFFER_SIZE){
        if ((*bpage=(PFbpage *)malloc(sizeof(PFbpage)))==NULL){
            *bpage = NULL;
            PFerrno = PFE_NOMEM;
            return(PFerrno);
        }
        PFnumbpage++;
    }
    else {
        *bpage = NULL;
        tbpage = choose_victim();
        if (tbpage == NULL){
            PFerrno = PFE_NOBUF;
            return(PFerrno);
        }

        if (tbpage->dirty) {
            PF_stats.physical_writes++;
            if ((error=(*writefcn)(tbpage->fd,tbpage->page,&tbpage->fpage))!= PFE_OK)
                return(error);
            tbpage->dirty = 0;
        }

        if ((error=PFhashDelete(tbpage->fd,tbpage->page))!= PFE_OK)
            return(error);

        PFbufUnlink(tbpage);
        *bpage = tbpage;
    }

    PFbufLinkHead(*bpage);
    return(PFE_OK);
}


/* PFbufGet with stats updates */
int PFbufGet(int fd,int pagenum,PFfpage **fpage,int (*readfcn)(),int (*writefcn)())
{
    PFbpage *bpage;
    int error;

    if ((bpage=PFhashFind(fd,pagenum)) == NULL){
        PF_stats.cache_misses++;

        if ((error=PFbufInternalAlloc(&bpage,writefcn))!= PFE_OK){
            *fpage = NULL;
            return(error);
        }

        /* physical read counted inside pfreadfcn, not here */

        if ((error=(*readfcn)(fd,pagenum,&bpage->fpage))!= PFE_OK){
            PFbufUnlink(bpage);
            PFbufInsertFree(bpage);
            *fpage = NULL;
            return(error);
        }

        if ((error=PFhashInsert(fd,pagenum,bpage))!=PFE_OK){
            PFbufUnlink(bpage);
            PFbufInsertFree(bpage);
            return(error);
        }

        bpage->fd = fd;
        bpage->page = pagenum;
        bpage->dirty = 0;
    }
    else {
        PF_stats.cache_hits++;
        PF_stats.logical_reads++;

        if (bpage->fixed){
            *fpage = &bpage->fpage;
            PFerrno = PFE_PAGEFIXED;
            return(PFerrno);
        }
    }

    bpage->fixed = 1;
    *fpage = &bpage->fpage;
    return(PFE_OK);
}

int PFbufUnfix(int fd,int pagenum,int dirty)
{
    PFbpage *bpage;

    if ((bpage= PFhashFind(fd,pagenum))==NULL){
        PFerrno = PFE_PAGENOTINBUF;
        return(PFerrno);
    }

    if (!bpage->fixed){
        PFerrno = PFE_PAGEUNFIXED;
        return(PFerrno);
    }

    if (dirty) {
        if (!bpage->dirty) {
            PF_stats.logical_writes++;
        }
        bpage->dirty = 1;
    }

    bpage->fixed = 0;

    PFbufUnlink(bpage);
    PFbufLinkHead(bpage);

    return(PFE_OK);
}

int PFbufAlloc(int fd,int pagenum,PFfpage **fpage,int (*writefcn)())
{
    PFbpage *bpage;
    int error;

    *fpage = NULL;

    if ((bpage=PFhashFind(fd,pagenum))!= NULL){
        PFerrno = PFE_PAGEINBUF;
        return(PFerrno);
    }

    if ((error=PFbufInternalAlloc(&bpage,writefcn))!= PFE_OK)
        return(error);

    if ((error=PFhashInsert(fd,pagenum,bpage))!= PFE_OK){
        PFbufUnlink(bpage);
        PFbufInsertFree(bpage);
        return(error);
    }

    bpage->fd = fd;
    bpage->page = pagenum;
    bpage->fixed = 1;
    bpage->dirty = 0;

    *fpage = &bpage->fpage;
    return(PFE_OK);
}

int PFbufReleaseFile(int fd,int (*writefcn)())
{
    PFbpage *bpage;
    PFbpage *temppage;
    int error;

    bpage = PFfirstbpage;
    while (bpage != NULL){
        if (bpage->fd == fd){
            if (bpage->fixed){
                PFerrno = PFE_PAGEFIXED;
                return(PFerrno);
            }

            if (bpage->dirty){
                PF_stats.physical_writes++;
                if ((error=(*writefcn)(fd,bpage->page,&bpage->fpage))!= PFE_OK)
                    return(error);
                bpage->dirty = 0;
            }

            if ((error=PFhashDelete(fd,bpage->page))!= PFE_OK){
                printf("Internal error:PFbufReleaseFile()\n");
                exit(1);
            }

            temppage = bpage;
            bpage = bpage->nextpage;
            PFbufUnlink(temppage);
            PFbufInsertFree(temppage);
        }
        else    bpage = bpage->nextpage;
    }
    return(PFE_OK);
}

int PFbufUsed(int fd,int pagenum)
{
    PFbpage *bpage;

    if ((bpage=PFhashFind(fd,pagenum))==NULL){
        PFerrno = PFE_PAGENOTINBUF;
        return(PFerrno);
    }

    if (!(bpage->fixed)){
        PFerrno = PFE_PAGEUNFIXED;
        return(PFerrno);
    }

    if (!bpage->dirty)
        PF_stats.logical_writes++;

    bpage->dirty = 1;

    PFbufUnlink(bpage);
    PFbufLinkHead(bpage);

    return(PFE_OK);
}

void PFbufPrint()
{
    PFbpage *bpage;

    printf("buffer content:\n");
    if (PFfirstbpage == NULL)
        printf("empty\n");
    else {
        printf("fd\tpage\tfixed\tdirty\tfpage\n");
        for(bpage = PFfirstbpage; bpage != NULL; bpage= bpage->nextpage)
            printf("%d\t%d\t%d\t%d\t%p\n",
                bpage->fd,bpage->page,(int)bpage->fixed,
                (int)bpage->dirty,(void *)&bpage->fpage);
    }
}
