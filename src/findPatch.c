#define min(a,b) \
       ({ __typeof__ (a) _a = (a); \
               __typeof__ (b) _b = (b); \
             _a < _b ? _a : _b; })
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void fileOpen(FILE **f, char *name) {
   *f = fopen(name, "r");
	if (f==NULL) {fprintf(stderr, "!!!!! Error opening %s !!!!! \n", name); exit(EXIT_FAILURE);}
}

struct Coord {
    int k; //la ligne
    int l; //la colonne
};

int minInst(int tl0k_1, int tl0k, int k, size_t n, int tl[n+1], int l, size_t m, struct Coord pred[n+1][m+1]) {
    int actual_min = min(tl0k_1, tl0k);

    pred[k][l].l = l-1;
    if(actual_min == tl0k_1) {//il s'agit d'une suppression
        pred[k][l].k = k-1;
    } else { //il s'agit d'un ajout
        pred[k][l].k = k;
    }
    for(int p=0; p<(k-1); p++) {
        int min_before = actual_min;
        actual_min = min(actual_min, 15 + tl[p]);
        if(actual_min < min_before) {
            //multi-destruction
            pred[k][l].k = p;
            pred[k][l].l = l;
        }
    }
    int min_before = actual_min;
    actual_min = min(actual_min, 10 + tl[k-1]);
    if(actual_min < min_before) {
        //multi-destruction
        pred[k][l].k = k-1;
        pred[k][l].l = l;
    }
    return actual_min;
}

void outputCoordMatrix(size_t n, size_t m, struct Coord pred[n+1][m+1]) {
    for(int i=0; i<=n; i++) {
        for(int j=0; j<=m; j++) {
            printf("(%i, %i) ", pred[i][j].k, pred[i][j].l);
        }
        printf("\n");
    }
}

void evaluateSucc(size_t n, size_t m, int start_succ, struct Coord succ[n+m+2], 
        int c[n][m], int lengthLineF2[m], int offLineF2[m], FILE *f2) {
    //on a besoin d'un compteur pour les destructions et les substitutions
    int f1_count = 0;
    int f2_count = 0;
    int k = 0;
    int l = 0;
    int k_succ = 0;
    int l_succ = 0;
    for(int i=start_succ+1; i<(n+m+2); i++) {
        k_succ = succ[i].k;
        l_succ = succ[i].l;
        //on regarde de quelle opération il s'agit
        if(k_succ == k) {
            //il s'agit d'un ajout
            printf("+ %i\n", k_succ);
            //on charge la ligne correspondante
            char f2_j[lengthLineF2[l_succ]];  
            fseek(f2, offLineF2[l_succ-1], SEEK_SET);
            fgets(f2_j, lengthLineF2[l_succ-1], f2);
            printf("%s\n", f2_j);
            f2_count++;
        }
        if((k_succ == (k+1)) && (l_succ == (l+1))) {
            //il s'agit d'une substitution
            if(c[f1_count][f2_count] != 0) {
                printf("= %i\n", k_succ);
                //on charge la ligne correspondante
                char f2_j[lengthLineF2[l_succ]];  
                fseek(f2, offLineF2[l_succ-1], SEEK_SET);
                fgets(f2_j, lengthLineF2[l_succ-1], f2);
                printf("%s\n", f2_j);
                f1_count++;
                f2_count++;
            } else {
                f2_count++;
                f1_count++;
            }
        }
        if(l_succ == l) {
            //il s'agit d'une destruction
            if(k_succ == (k+1)) {
                //destruction simple
                printf("d %i\n", ++f1_count);
            } else {
                printf("D %i %i\n", f1_count+1, k_succ-k);
                f1_count += k_succ -k;
            }
        }
        k = k_succ;
        l = l_succ;
    }
}

void findPath(size_t n, size_t m, struct Coord pred[n+1][m+1], int c[n][m], 
        int lengthLineF2[m], int offLineF2[m], FILE *f2) {
    struct Coord pred_tmp;
    pred_tmp.k = pred[n][m].k;
    pred_tmp.l = pred[n][m].l;
    int k, l;
    //on va stocker les successeurs dans un tableau
    struct Coord succ[n+m+2];
    int start_succ = n+m+1;

    k = n;
    l = m;
    while((pred_tmp.k!=0) &&(pred_tmp.l!=0)) {
        succ[start_succ].k = k;
        succ[start_succ--].l = l;
        k = pred_tmp.k;
        l = pred_tmp.l;
        pred_tmp.k = pred[k][l].k;
        pred_tmp.l = pred[k][l].l;
    }
    succ[start_succ].k = k;
    succ[start_succ--].l = l;
    succ[start_succ].k = pred_tmp.k;
    succ[start_succ--].l = pred_tmp.l;
    succ[start_succ].k = 0;
    succ[start_succ].l = 0;
    evaluateSucc(n, m, start_succ, succ, c, lengthLineF2, offLineF2, f2);
}

void findPatch(size_t n, size_t m, int c[n][m],  int lengthLineF2[m], 
        int offLineF2[m], FILE *f2) {
    int tl0[n+1];
    int tl[n+1];
    //pour pouvoir repérer le chemin, on va utiliser une matriced de 
    //prédecesseurs
    struct Coord pred[n+1][m+1];
    for(int i=0; i<=n; i++) {
        for(int j=0; j<=m; j++) {
            pred[i][j].k = -1;
            pred[i][j].l = -1;
        }
    }

    //CI
    tl0[0] = 0;
    pred[0][0].k = 0;
    pred[0][0].l = 0;

    tl0[1] = 10;
    pred[1][0].k = 0;
    pred[1][0].l = 0;

    for(int k=2; k<=n; k++) {
        tl0[k] = 15;
        pred[k][0].k = 0;
        pred[k][0].l = 0;
    }
    
    for(int l=1; l<=m; l++) {
        tl[0] = tl0[0] + 10 + lengthLineF2[0]; //dans l2, on commence avec la 1ère ligne
        pred[0][l].k = 0;
        pred[0][l].l = l-1;
        for(int k=1; k<=n; k++) {
            //c'est ici qu'on implémente l'équation de bellman
            tl[k] = minInst(tl0[k-1] + c[k-1][l-1], tl0[k] + 10 + lengthLineF2[l-1], k, n, tl, l, m, pred);
        }
        //recopie
        for(int q=0; q <(n+1); q++) {
            tl0[q] = tl[q];
        }
    }
//    printf("%i\n", tl[n]);
    findPath(n, m, pred, c, lengthLineF2, offLineF2, f2);
}

int main(int argc, char *argv[]) {
    int c[6][7] = {{15, 0, 12, 14, 12, 12, 13}, {15, 13, 12, 0, 12, 12, 13}, {15, 13, 12, 14, 12, 12, 13}, {15, 13, 12, 14, 12, 12, 13}, {15, 13, 12, 14, 12, 12, 13}, {15, 13, 12, 14, 12, 0, 13} };

    int L[7] = {5, 4, 2, 4, 2, 2, 3};
    int offLineF2[7] = {0, 5, 9, 11, 15, 17, 19};
    FILE *f2; 
    fileOpen(&f2, "f2.txt");
    findPatch(6, 7, c, L, offLineF2, f2);
    return 0;
}
