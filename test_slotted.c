#include <stdio.h>
#include <string.h>

#include "pftypes.h"   /* PF_PAGE_SIZE and PFfpage */
#include "spage.h"   /* slotted page routines */

int main(void)
{
    printf("Starting basic slotted-page check...\n");

    /* one page of PFfpage storage */
    PFfpage page;
    SL_InitPage(page.pagebuf);

    int a, b, c;

    /* insert three short records */
    SL_InsertRecord(page.pagebuf, "hello", 5, &a);
    SL_InsertRecord(page.pagebuf, "world", 5, &b);
    SL_InsertRecord(page.pagebuf, "toydb", 5, &c);

    printf("Allocated slots: %d, %d, %d\n", a, b, c);

    char tmp[128];
    int length = 0;

    /* fetch each record back */
    if (SL_GetRecord(page.pagebuf, a, tmp, &length) == 0) {
        tmp[length] = '\0';
        printf("slot %d => %s\n", a, tmp);
    }

    if (SL_GetRecord(page.pagebuf, b, tmp, &length) == 0) {
        tmp[length] = '\0';
        printf("slot %d => %s\n", b, tmp);
    }

    if (SL_GetRecord(page.pagebuf, c, tmp, &length) == 0) {
        tmp[length] = '\0';
        printf("slot %d => %s\n", c, tmp);
    }

    printf("Slotted-page test finished.\n");
    return 0;
}
