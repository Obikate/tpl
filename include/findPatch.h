#ifndef __FINDPATCH_H__
#define __FINDPATCH_H__

#include<stdint.h>
#include<stdio.h>

extern int findPatch(size_t n, size_t m, int **c,  int lengthLineF2[m], 
        int offLineF2[m], FILE *f2);

#endif
