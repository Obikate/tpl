#ifndef __TREATFILES_H__
#define __TREATFILES_H__

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

extern void fileOpen(FILE **f, char *name);

extern int getLengthFromFile(FILE *f);

extern void treatFiles(FILE *f1, FILE *f2, char *name1, char *name2, 
        size_t n, size_t m, int **c, int offLineF2[m], int lengthLineF2[m]);

#endif
